//
// Created by samuel on 09/10/22.
//

#include "ImGui/IconsFontAwesome5.hpp"
#include "ImGui/IconsFontAwesome5Brands.hpp"

#include "ConsolePanel.hpp"
#include "glm/exponential.hpp"
#include "Events/EventSystem.hpp"
#include "EditorEvent.hpp"

namespace fox
{
    ConsolePanel::ConsolePanel(const char* name)
    {
//        FOX_PROFILE_SCOPE();

        s_MessageBufferRenderFilter |= ConsolePanel::loglevel::INFO;
        s_MessageBufferRenderFilter |= ConsolePanel::loglevel::WARN;
        s_MessageBufferRenderFilter |= ConsolePanel::loglevel::ERROR;

        event::EventSystem::Get().On<OnConsoleLogEvent>(FOX_BIND_EVENT_FN(ConsolePanel::OnLog));
        m_MessageBuffer = std::vector<Ref<ConsolePanel::Message>>(m_Capacity);
    }

    ConsolePanel::~ConsolePanel()
    {
    }

    void ConsolePanel::OnLog(const OnConsoleLogEvent& event)
    {
        AddMessage(event.Message, event.Log);
    }

    void ConsolePanel::AddMessage(const std::string& message, ConsolePanel::loglevel level)
    {
//        FOX_PROFILE_SCOPE();

        static uint64_t id = 0;

        std::string strId = std::to_string(id).c_str();
        *(m_MessageBuffer.begin() + m_BufferBegin) = new_ref<Message>(strId, message, level);
        if (++m_BufferBegin == m_Capacity)
            m_BufferBegin = 0;
        if (m_BufferSize < m_Capacity)
            m_BufferSize++;

        // Request to scroll to bottom of the list to view the new message
        m_RequestScrollToBottom = m_AllowScrollingToBottom;

        id++;
    }

    const ConsolePanel::Message* ConsolePanel::GetRecentMessage()
    {
//        FOX_PROFILE_SCOPE();

        if (m_BufferBegin == 0)
            return nullptr;

        return (m_MessageBuffer.begin() + m_BufferBegin - 1)->Raw();
    }

    void ConsolePanel::Clear()
    {
//        FOX_PROFILE_SCOPE();

        for (auto& message : m_MessageBuffer)
            message = nullptr;

        m_BufferBegin = 0;
    }

    void ConsolePanel::OnImGui(bool& isOpen)
    {
//        FOX_PROFILE_SCOPE();

        if (ImGui::Begin(ICON_FA_TERMINAL" Console", &isOpen))
        {
            ImGuiRenderHeader();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGuiRenderMessages();

            ImGui::End();
        }
    }

    void ConsolePanel::SetFocus() const
    {
//        FOX_PROFILE_SCOPE();

        ImGui::SetWindowFocus(ICON_FA_TERMINAL" Console");
    }

    void ConsolePanel::ImGuiRenderHeader()
    {
//        FOX_PROFILE_SCOPE();

        if (ImGui::Button(ICON_FA_COGS))
            ImGui::OpenPopup("SettingsPopup");

        if (ImGui::BeginPopup("SettingsPopup"))
        {
            ImGuiRenderSettings();
            ImGui::EndPopup();
        }

        ImGui::SameLine();

        float spacing = ImGui::GetStyle().ItemSpacing.x;
        ImGui::GetStyle().ItemSpacing.x = 2;
        ImVec2 vec = { ImGui::GetStyle().FramePadding.x * 2.0f, ImGui::GetStyle().FramePadding.y * 2.0f };
        float levelButtonWidth = (ImGui::CalcTextSize(Message::GetLevelIcon(ConsolePanel::loglevel(1))).x + vec.x);
        float levelButtonWidths = (levelButtonWidth + ImGui::GetStyle().ItemSpacing.x) * 7;

        const float cursorPosX = ImGui::GetCursorPosX();
        m_Filter.Draw("###ConsoleFilter", ImGui::GetContentRegionAvail().x - levelButtonWidths);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        for(int i = 0; i < 3; i++)
        {
            ImGui::SameLine();
            auto level = ConsolePanel::loglevel(glm::pow(2, i));

            bool levelEnabled = s_MessageBufferRenderFilter & level;
            glm::vec4 c = Message::GetRenderColor(level);
            if(levelEnabled)
                ImGui::PushStyleColor(ImGuiCol_Text, { c.r, c.g, c.b, c.a });
            else
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5, 0.5f, 0.5f));

            if(ImGui::Button(Message::GetLevelIcon(level)))
            {
                s_MessageBufferRenderFilter ^= level;
            }

            if(ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("%s", Message::GetLevelName(level));
            }
            ImGui::PopStyleColor();
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_TIMES))
            Clear();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::GetStyle().ItemSpacing.x = spacing;

        if (!m_Filter.IsActive())
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(cursorPosX + ImGui::GetFontSize() * 0.5f);
            ImGui::TextUnformatted(ICON_FA_SEARCH" Search...");
        }
    }

    static uint32_t s_Counter = 0;
    static char s_IDBuffer[16];

    void BeginProperties(ImGuiTableFlags flags)
    {
        s_IDBuffer[0] = '#';
        s_IDBuffer[1] = '#';
        memset(s_IDBuffer + 2, 0, 14);
        ++s_Counter;
        std::string s = std::to_string(s_Counter);
        strncpy(s_IDBuffer + 2, s.c_str(), 14);

        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_PadOuterX;
        ImGui::BeginTable(s_IDBuffer, 2, tableFlags | flags);
        ImGui::TableSetupColumn("PropertyName", 0, 0.5f);
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthStretch);
    }

    void EndProperties()
    {
        ImGui::EndTable();
    }

    void ConsolePanel::ImGuiRenderSettings()
    {
//        FOX_PROFILE_SCOPE();

        BeginProperties(ImGuiTableFlags_SizingStretchSame);
        ImGui::Checkbox("Scroll to bottom", &m_AllowScrollingToBottom);
        ImGui::DragFloat("DisplayScale", &m_DisplayScale, 0.5f, 4.0f);
        EndProperties();
    }

    void ConsolePanel::ImGuiRenderMessages()
    {
//        FOX_PROFILE_SCOPE();

        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg
                                               | ImGuiTableFlags_ContextMenuInBody
                                               | ImGuiTableFlags_ScrollX
                                               | ImGuiTableFlags_ScrollY;

        if (ImGui::BeginTable("ScrollRegionTable", 1, tableFlags))
        {
            ImGui::SetWindowFontScale(m_DisplayScale);

            const auto& messageStart = m_MessageBuffer.begin() + m_BufferBegin;
            if (*messageStart) // If contains old message here
            {
                for (auto message = messageStart; message != m_MessageBuffer.end(); message++)
                {
                    if (!(s_MessageBufferRenderFilter & (*message)->Level))
                        continue;

                    if (m_Filter.IsActive())
                    {
                        const auto& m = *message;
                        if (m_Filter.PassFilter(m->Buffer.c_str()))
                        {
                            m->OnImGuiRender();
                        }
                    }
                    else
                    {
                        (*message)->OnImGuiRender();
                    }
                }
            }

            if (m_BufferBegin != 0) // Skipped first messages in vector
            {
                for (auto message = m_MessageBuffer.begin(); message != messageStart; message++)
                {
                    if (!(s_MessageBufferRenderFilter & (*message)->Level))
                        continue;

                    if(m_Filter.IsActive())
                    {
                        if(m_Filter.PassFilter((*message)->Buffer.c_str()))
                        {
                            (*message)->OnImGuiRender();
                        }
                    }
                    else
                    {
                        (*message)->OnImGuiRender();
                    }
                }
            }

            if (m_RequestScrollToBottom && ImGui::GetScrollMaxY() > 0)
            {
                ImGui::SetScrollY(ImGui::GetScrollMaxY());
                m_RequestScrollToBottom = false;
            }

            ImGui::EndTable();
        }
    }

    ConsolePanel::Message::Message(const std::string& id, const std::string& message, ConsolePanel::loglevel level)
        : ID(id), Buffer(message), Level(level)
    {
//        FOX_PROFILE_SCOPE();

    }

    void ConsolePanel::Message::OnImGuiRender() const
    {
//        FOX_PROFILE_SCOPE();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        glm::vec4 c = GetRenderColor(Level);
        ImGui::PushStyleColor(ImGuiCol_Text, { c.r, c.g, c.b, c.a });
        auto levelIcon = GetLevelIcon(Level);
        ImGui::Text("%s  %s", levelIcon, Buffer.c_str());
        ImGui::PopStyleColor();

        if(ImGui::BeginPopupContextItem(ID.c_str()))
        {
            if(ImGui::MenuItem("Copy"))
                ImGui::SetClipboardText(Buffer.c_str());

            ImGui::EndPopup();
        }
    }

    const char* ConsolePanel::Message::GetLevelName(ConsolePanel::loglevel level)
    {
        return Logger::get_type_str(level);
    }

    glm::vec4 ConsolePanel::Message::GetRenderColor(ConsolePanel::loglevel level)
    {
        switch (level)
        {
            case ConsolePanel::loglevel::INFO:			return { 0.20f, 0.80f, 0.20f, 1.00f }; // Green
            case ConsolePanel::loglevel::WARN:			return { 0.80f, 0.80f, 0.20f, 1.00f }; // Yellow
            case ConsolePanel::loglevel::ERROR:			return { 0.90f, 0.25f, 0.25f, 1.00f }; // Red
        }

        return { 1.00f, 1.00f, 1.00f, 1.00f };
    }

    const char* ConsolePanel::Message::GetLevelIcon(ConsolePanel::loglevel level)
    {
        switch(level)
        {
            case ConsolePanel::loglevel::INFO:				return ICON_FA_INFO;
            case ConsolePanel::loglevel::WARN:				return ICON_FA_EXCLAMATION_TRIANGLE;
            case ConsolePanel::loglevel::ERROR:				return ICON_FA_EXCLAMATION;
        }

        return "Unknown name";
    }
}
