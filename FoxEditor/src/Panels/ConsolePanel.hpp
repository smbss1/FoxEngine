//
// Created by samuel on 09/10/22.
//

#ifndef FOX_LEXER_CONSOLEPANEL_HPP
#define FOX_LEXER_CONSOLEPANEL_HPP

#include "Panel.hpp"
#include "Core/Logger/Logger.hpp"
#include "imgui.h"
#include "glm/vec4.hpp"
#include "common.hpp"
#include <string>
#include <vector>

namespace fox
{
    struct OnConsoleLogEvent;

    class ConsolePanel : public Panel
    {
        using loglevel = typelog;
    public:
        struct Message : public RefCounted
        {
            std::string ID;
            std::string Buffer;
            loglevel Level;

            Message(const std::string& id, const std::string& message = "", loglevel level = loglevel::INFO);
            void OnImGuiRender() const;

            static const char* GetLevelName(loglevel level);
            static glm::vec4 GetRenderColor(loglevel level);
            static const char* GetLevelIcon(loglevel level);
        };

    public:
        explicit ConsolePanel(const char* name = "Console");
        virtual ~ConsolePanel();

        ConsolePanel(const ConsolePanel& other) = delete;
        ConsolePanel(ConsolePanel&& other) = delete;
        ConsolePanel& operator=(const ConsolePanel& other) = delete;
        ConsolePanel& operator=(ConsolePanel&& other) = delete;

        void OnLog(const OnConsoleLogEvent& event);
        void AddMessage(const std::string& message, loglevel level);
        const Message* GetRecentMessage();
        void Clear();
        void SetFocus() const;

        virtual void OnImGui() override;

    private:
        void ImGuiRenderHeader();
        void ImGuiRenderSettings();
        void ImGuiRenderMessages();

    private:
        float m_DisplayScale = 1.0f;
        uint16_t m_Capacity = 200;
        uint16_t m_BufferSize = 0;
        uint16_t m_BufferBegin = 0;
        uint32_t s_MessageBufferRenderFilter = loglevel::INFO;
        bool m_AllowScrollingToBottom = true;
        bool m_RequestScrollToBottom = false;
        std::vector<fox::Ref<Message>> m_MessageBuffer;
        ImGuiTextFilter m_Filter;
    };
}

#endif //FOX_LEXER_CONSOLEPANEL_HPP
