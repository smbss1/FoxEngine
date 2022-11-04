//
// Created by samuel on 24/10/22.
//

#include "ImGuiExtensions.hpp"
#include "Core/Project.hpp"
#include "Asset/AssetManager.hpp"
#include "Renderer/Texture.hpp"
#include "imgui_internal.h"
#include "ImGuiExtension.hpp"

#include <cstdlib>

namespace fox::UI
{
    static int s_UIContextID = 0;
    static uint32_t s_Counter = 0;
    static char s_IDBuffer[16];

    void PushID()
    {
        ++s_UIContextID;
        ImGui::PushID(s_UIContextID);
        s_Counter = 0;
    }

    const char* GenerateID()
    {
        s_IDBuffer[0] = '#';
        s_IDBuffer[1] = '#';
        memset(s_IDBuffer + 2, 0, 14);
        ++s_Counter;
        std::string s = std::to_string(s_Counter);
        strncpy(s_IDBuffer + 2, s.c_str(), 14);
        return s_IDBuffer;
    }

    void PopID()
    {
        ImGui::PopID();
        --s_UIContextID;
    }

    void BeginProperties(ImGuiTableFlags flags)
    {
        PushID();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
        ImGui::Columns(2);
    }

    void EndProperties()
    {
        ImGui::Columns(1);
//        UI::Draw::Underline();
        ImGui::PopStyleVar(2); // ItemSpacing, FramePadding
        UI::ShiftCursorY(18.0f);
        PopID();
    }

    void BeginPropertyGrid(const char* label, const char* tooltip, bool rightAlignNextColumn)
    {
        ShiftCursor(10.0f, 9.0f);
        ImGui::Text(label);
//        if (std::strlen(helpText) != 0)
//        {
//            ImGui::SameLine();
////            HelpMarker(helpText);
//        }
        ImGui::NextColumn();
        ShiftCursorY(4.0f);
        ImGui::PushItemWidth(-1);
        GenerateID();
    }

    void EndPropertyGrid()
    {
//        if (!IsItemDisabled())
//            DrawItemActivityOutline(2.0f, true, IM_COL32(236, 158, 36, 255));

        ImGui::PopItemWidth();
        ImGui::NextColumn();
//        Draw::Underline();
    }

    //=========================================================================================
    /// Cursor

    void ShiftCursorX(float distance)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + distance);
    }

    void ShiftCursorY(float distance)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance);
    }

    void ShiftCursor(float x, float y)
    {
        const ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y + y));
    }

//    void BeginDisabled(bool disabled = true)
//    {
//        if (disabled)
//            ImGui::BeginDisabled(true);
//    }

    ImRect GetItemRect()
    {
        return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    inline ImRect RectExpanded(const ImRect& rect, float x, float y)
    {
        ImRect result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }

    void DrawItemActivityOutline(float rounding, bool drawWhenInactive, ImColor colourWhenActive)
    {
        auto* drawList = ImGui::GetWindowDrawList();
        const ImRect rect = RectExpanded(GetItemRect(), 1.0f, 1.0f);
        if (ImGui::IsItemHovered() && !ImGui::IsItemActive())
        {
            drawList->AddRect(rect.Min, rect.Max,
                              ImColor(60, 60, 60), rounding, 0, 1.5f);
        }
        if (ImGui::IsItemActive())
        {
            drawList->AddRect(rect.Min, rect.Max,
                              colourWhenActive, rounding, 0, 1.0f);
        }
        else if (!ImGui::IsItemHovered() && drawWhenInactive)
        {
            drawList->AddRect(rect.Min, rect.Max,
                              ImColor(50, 50, 50), rounding, 0, 1.0f);
        }
    };


    bool IsItemDisabled()
    {
        return ImGui::GetItemFlags() & ImGuiItemFlags_Disabled;
    }

//    void EndDisabled()
//    {
//        // NOTE(Peter): Cheeky hack to prevent ImGui from asserting (required due to the nature of UI::BeginDisabled)
//        if (GImGui->DisabledStackSize > 0)
//            ImGui::EndDisabled();
//    }

    bool Checkbox(const std::string& label, bool& value)
    {
        return ImGui::Checkbox(label.c_str(), &value);
    }

    bool DragScalar(const std::string& label, uint32_t& value, float speed, uint32_t min, uint32_t max)
    {
        return ImGui::DragScalar(label.c_str(), ImGuiDataType_U32, &value, speed, &min, &max, "%u");
    }

    bool DragScalar(const std::string& label, int32_t &value, float speed, int32_t min, int32_t max)
    {
        return ImGui::DragScalar(label.c_str(), ImGuiDataType_S32, &value, speed, &min, &max, "%d");
    }

    bool DragScalar(const std::string& label, uint64_t &value, float speed, uint64_t min, uint64_t max)
    {
        return ImGui::DragScalar(label.c_str(), ImGuiDataType_U64, &value, speed, &min, &max, "%llu");
    }

    bool DragScalar(const std::string& label, int64_t &value, float speed, int64_t min, int64_t max)
    {
        return ImGui::DragScalar(label.c_str(), ImGuiDataType_S64, &value, speed, &min, &max, "%lld");
    }

    bool DragScalar(const std::string& label, float& value, float speed, float min, float max)
    {
        return ImGui::DragScalar(label.c_str(), ImGuiDataType_Float, &value, speed, &min, &max, "%.3f");
    }

    bool DragScalar(const std::string& label, double &value, float speed, double min, double max)
    {
        return ImGui::DragScalar(label.c_str(), ImGuiDataType_Double, &value, speed, &min, &max, "%.6f");
    }

    bool DragScalarN(const std::string& label, uint32_t *value, int components, float speed, uint32_t min, uint32_t max)
    {
        return ImGui::DragScalarN(label.c_str(), ImGuiDataType_U32, value, components, speed, &min, &max, "%u");
    }

    bool DragScalarN(const std::string& label, int32_t *value, int components, float speed, int32_t min, int32_t max)
    {
        return ImGui::DragScalarN(label.c_str(), ImGuiDataType_S32, value, components, speed, &min, &max, "%d");
    }

    bool DragScalarN(const std::string& label, uint64_t *value, int components, float speed, uint64_t min, uint64_t max)
    {
        return ImGui::DragScalarN(label.c_str(), ImGuiDataType_U64, value, components, speed, &min, &max, "%llu");
    }

    bool DragScalarN(const std::string& label, int64_t *value, int components, float speed, int64_t min, int64_t max)
    {
        return ImGui::DragScalarN(label.c_str(), ImGuiDataType_S64, value, components, speed, &min, &max, "%lld");
    }

    bool DragScalarN(const std::string& label, float *value, int components, float speed, float min, float max)
    {
        return ImGui::DragScalarN(label.c_str(), ImGuiDataType_Float, value, components, speed, &min, &max, "%.3f");
    }

    bool DragScalarN(const std::string& label, double *value, int components, float speed, double min, double max)
    {
        return ImGui::DragScalarN(label.c_str(), ImGuiDataType_Double, value, components, speed, &min, &max, "%.3f");
    }

    bool DragScalar(const std::string& label, glm::vec3& value, float speed, float min, float max)
    {
        return imgui::DrawVec3Control(label, value);
    }

    bool Slider(const std::string& label, double& value, double min, double max, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalar(label.c_str(), ImGuiDataType_Double, &value, &min, &max, "%.6f", flags);
    }

    bool Slider(const std::string& label, float& value, float min, float max, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalar(label.c_str(), ImGuiDataType_Float, &value, &min, &max, "%.3f", flags);
    }

    bool Slider(const std::string& label, uint32_t& value, uint32_t min, uint32_t max, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalar(label.c_str(), ImGuiDataType_U32, &value, &min, &max, "%u", flags);
    }

    bool Slider(const std::string& label, uint64_t& value, uint64_t min, uint64_t max, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalar(label.c_str(), ImGuiDataType_U64, &value, &min, &max, "%llu", flags);
    }

    bool Slider(const std::string& label, int32_t& value, int32_t min, int32_t max, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalar(label.c_str(), ImGuiDataType_S32, &value, &min, &max, "%d", flags);
    }

    bool Slider(const std::string& label, int64_t& value, int64_t min, int64_t max, ImGuiSliderFlags flags)
    {
        return ImGui::SliderScalar(label.c_str(), ImGuiDataType_S64, &value, &min, &max, "%lld", flags);
    }

    bool AssetField(const std::string& label, AssetHandle& value, bool handlePayload, const std::vector<std::string>& extensions)
    {
        UI::BeginPropertyGrid(label.c_str(), nullptr);

        Ref<Asset> asset = AssetManager::GetAsset<Asset>(value);
        bool result = ImGui::Button(asset ? AssetManager::GetFileSystemPath(AssetManager::GetMetadata(value)).stem().c_str() : "None");
        UI::EndPropertyGrid();

        if (handlePayload)
        {
            HandleContentBrowserPayload(value, extensions);
        }
        return result;
    }

    void PreviewTexture(AssetHandle& value, const std::vector<std::string>& extensions)
    {
        Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(value);
        if (!texture)
        {
            return;
        }

        ImVec2 p = ImGui::GetCursorPos();
        const ImVec2 boxSize = ImVec2(100, 100);

        ImGui::SetCursorPos(ImVec2(p.x + (boxSize.x * 0.5f), p.y));
        ImGui::Image(reinterpret_cast<ImTextureID>(texture->GetRendererID()), boxSize);
        HandleContentBrowserPayload(value, extensions);
    }

    bool HandleContentBrowserPayload(AssetHandle& handle, const std::vector<std::string>& extensions)
    {
        return HandleContentBrowserPayloadCustom(extensions, [&handle](std::filesystem::path& filepath) {
            handle = AssetManager::GetAssetHandleFromFilePath(filepath);
        });
    }

    bool HandleContentBrowserPayloadCustom(const std::vector<std::string>& extensions, std::function<void(std::filesystem::path&)> func)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *path = (const char *) payload->Data;
                std::filesystem::path prefabPath = Project::AssetsDir() / path;

                for (auto& extension : extensions)
                {
                    if (prefabPath.extension() == extension)
                    {
                        func(prefabPath);
                        return true;
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        return false;
    }

    bool ImageButton(AssetHandle& handle)
    {
        Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
        if (!texture)
            return false;
        return ImGui::ImageButton(reinterpret_cast<ImTextureID>(texture->GetRendererID()), ImVec2(100, 100));
    }

    bool DropBoxAsset(AssetHandle& handle, const std::vector<std::string>& extensions)
    {
//        ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
//        const ImVec2 boxSize = ImVec2(ImGui::GetContentRegionAvail().x, 70);
//        ImVec2 p = ImGui::GetCursorPos();
//        ImGui::SetCursorPos(ImVec2(p.x, p.y + spacing.y));
//        p = ImGui::GetCursorPos();
//        ImGui::InvisibleButton("###drop_box", boxSize);
//        ImGui::SetItemAllowOverlap();
//        UI::HandleContentBrowserPayload(handle, extensions);
//
//        ImVec2 size = ImGui::CalcTextSize("Drop a texture here...");
//        ImGui::SetCursorPos(ImVec2(p.x + (boxSize.x * 0.5f) - (size.x * 0.5f), p.y + (boxSize.y * 0.5f) - (ImGui::GetTextLineHeight() * 0.5f)));
//        p.y = ImGui::GetCursorPos().y;
//        ImGui::Text("Drop a texture here...");
//        ImGui::SetCursorPos(ImVec2(p.x, p.y + boxSize.y));

        ImVec2 boxSize = ImVec2(ImGui::GetContentRegionAvail().x, 70);
        ImGui::BeginChild("Test", boxSize, true);
        {
            const ImVec2 region = ImGui::GetContentRegionAvail();
            ImVec2 size = ImGui::CalcTextSize("Drop a texture here...");
            ShiftCursor((region.x * 0.5f) - (size.x * 0.5f), (region.y * 0.5f) - (ImGui::GetTextLineHeight() * 0.5f));
            ImGui::Text("Drop a texture here...");
        }
        ImGui::EndChild();
        UI::HandleContentBrowserPayload(handle, extensions);
    }

    bool Property(const std::string& name, const char* typeString[], int size, int index, int& valueFrom)
    {
        UI::BeginPropertyGrid(name.c_str(), nullptr);
        bool result = Enum(s_IDBuffer, typeString, size, index, valueFrom);
        UI::EndPropertyGrid();
        return result;
    }

    bool Enum(const std::string& name, const char* typeString[], int size, int index, int& valueFrom)
    {
        bool result = false;
        const char* currentTypeString = typeString[index];

        if (ImGui::BeginCombo(name.c_str(), currentTypeString))
        {
            for (int i = 0; i < size; ++i)
            {
                bool isSelected = currentTypeString == typeString[i];
                if (ImGui::Selectable(typeString[i], isSelected))
                {
                    currentTypeString = typeString[i];
                    valueFrom = i;
                    result = true;
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        return result;
    }

    bool Property(const std::string& label, bool& value, const char* helpText)
    {
        UI::BeginPropertyGrid(label.c_str(), nullptr);
        bool result = ToggleButton(s_IDBuffer, value);
        UI::EndPropertyGrid();
        return result;
    }

    bool InputText(const std::string& label, char* value, size_t size, ImGuiInputTextFlags flags)
    {
        ImGui::Text(label.c_str());
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.08f,0.08f,0.08f,1.0f });
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
        ImGui::PushID(label.c_str());

        bool result = ImGui::InputText(("##" + label).c_str(), value, size, flags);
        ImGui::PopStyleColor();
        ImGui::PopID();
        return result;
    }

    bool PropertyInput(const std::string& label, char* value, size_t size, ImGuiInputTextFlags flags)
    {
        UI::BeginPropertyGrid(label.c_str(), nullptr);
        bool result = ImGui::InputText(s_IDBuffer, value, size);
        UI::EndPropertyGrid();
        return result;
    }

    bool ToggleButton(const std::string& str_id, bool& v)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float height = ImGui::GetFrameHeight() * 0.85f;
        float width = height * 1.55f;
        float radius = height * 0.50f;

        ImGui::InvisibleButton(str_id.c_str(), ImVec2(width, height));
        bool result = ImGui::IsItemClicked();
        if (result)
            v = !v;

        float t = v ? 1.0f : 0.0f;

        ImGuiContext& g = *GImGui;
        float ANIM_SPEED = 0.08f;
        if (g.LastActiveId == g.CurrentWindow->GetID(str_id.c_str()))// && g.LastActiveIdTimer < ANIM_SPEED)
        {
            float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
            t = v ? (t_anim) : (1.0f - t_anim);
        }

        ImU32 col_bg;
        if (ImGui::IsItemHovered())
            col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
        else
            col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
        draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
        return result;
    }
}
