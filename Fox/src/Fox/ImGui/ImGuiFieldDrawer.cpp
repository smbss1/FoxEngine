//
// Created by samuel on 07/10/22.
//

#include <filesystem>
#include "Reflection/Core/Reflect.hpp"

#include "ImGuiFieldDrawer.hpp"
#include "imgui.h"
#include "glm/vec4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "common.hpp"
#include "Renderer/Texture.hpp"
#include "ImGuiExtension.hpp"
#include "Components/Rigidbody2D.hpp"
#include "Scripting/ScriptEngine.hpp"

namespace fox
{
    extern const std::filesystem::path g_AssetPath;

    std::unordered_map<const Reflect::TypeDescriptor *, std::function<bool(std::string, Reflect::Any&)>> ImGuiFieldDrawer::m_Drawers = {
        {Reflect::Resolve<int>(),                       ImGuiFieldDrawer::DrawInt},
        {Reflect::Resolve<float>(),                     ImGuiFieldDrawer::DrawFloat},
        {Reflect::Resolve<bool>(),                      ImGuiFieldDrawer::DrawBool},
        {Reflect::Resolve<glm::vec2>(),                 ImGuiFieldDrawer::DrawVec2},
        {Reflect::Resolve<glm::vec3>(),                 ImGuiFieldDrawer::DrawVec3},
        {Reflect::Resolve<glm::vec4>(),                 ImGuiFieldDrawer::DrawVec4},
        {Reflect::Resolve<fox::ref<fox::Texture2D>>(),  ImGuiFieldDrawer::DrawTexture},

        {Reflect::Resolve<Rigidbody2D::BodyType>(),     ImGuiFieldDrawer::DrawRigidbody2D_BodyType},
    };

    std::unordered_map<ScriptFieldType, std::function<bool(std::string, Reflect::Any&)>> ImGuiFieldDrawer::m_FieldDrawers = {
        {ScriptFieldType::Int,                          ImGuiFieldDrawer::DrawInt},
        {ScriptFieldType::Float,                     ImGuiFieldDrawer::DrawFloat},
        {ScriptFieldType::Bool,                      ImGuiFieldDrawer::DrawBool},
        {ScriptFieldType::Vector2,                 ImGuiFieldDrawer::DrawVec2},
        {ScriptFieldType::Vector3,                 ImGuiFieldDrawer::DrawVec3},
        {ScriptFieldType::Vector4,                 ImGuiFieldDrawer::DrawVec4},
//        {ScriptFieldType::Entity,                 ImGuiFieldDrawer::DrawEntityRef},
//        {ScriptFieldType::Texture,  ImGuiFieldDrawer::DrawTexture},

//        {ScriptFieldType::Rigidbody_BodyType,     ImGuiFieldDrawer::DrawRigidbody2D_BodyType},
    };

    int ImGuiFieldDrawer::s_UIContextID = 0;
    uint32_t ImGuiFieldDrawer::s_Counter = 0;
    char ImGuiFieldDrawer::s_IDBuffer[16];

    void ImGuiFieldDrawer::PushID()
    {
        ++s_UIContextID;
        ImGui::PushID(s_UIContextID);
        s_Counter = 0;
    }

    void ImGuiFieldDrawer::PopID()
    {
        ImGui::PopID();
        --s_UIContextID;
    }

    void ImGuiFieldDrawer::BeginProperties(ImGuiTableFlags flags)
    {
        constexpr ImGuiTableFlags tableFlags = 0;
        ImGui::BeginTable(s_IDBuffer, 2, tableFlags | flags);
        ImGui::TableSetupColumn("PropertyName", 0, 0.5f);
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthStretch);
    }

    void ImGuiFieldDrawer::EndProperties()
    {
        ImGui::EndTable();
    }

    void ImGuiFieldDrawer::BeginPropertyGrid(const char* label, const char* tooltip, bool rightAlignNextColumn)
    {
        PushID();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::PushID(label);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);
        ImGui::Text(label);
        if (tooltip && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text(tooltip);
            ImGui::EndTooltip();
        }

        ImGui::TableNextColumn();

        if (rightAlignNextColumn)
            ImGui::SetNextItemWidth(-FLT_MIN);

        s_IDBuffer[0] = '#';
        s_IDBuffer[1] = '#';
        memset(s_IDBuffer + 2, 0, 14);
        ++s_Counter;
        std::string s = std::to_string(s_Counter);
        strncpy(s_IDBuffer + 2, s.c_str(), 14);
    }

    void ImGuiFieldDrawer::EndPropertyGrid()
    {
        ImGui::PopID();
        PopID();
    }

    bool ImGuiFieldDrawer::Draw(const Reflect::TypeDescriptor *type, std::string name, Reflect::Any& value)
    {
        try {
            ImGuiFieldDrawer::BeginPropertyGrid(name.c_str(), nullptr);
            bool result = m_Drawers.at(type)(name, value);
            ImGuiFieldDrawer::EndPropertyGrid();
            return result;
        }
        catch (std::exception exception) {
            return false;
        }
    }

    bool ImGuiFieldDrawer::DrawField(ScriptFieldType type, std::string name, Reflect::Any& value)
    {
        try {
            ImGuiFieldDrawer::BeginPropertyGrid(name.c_str(), nullptr);
            bool result = m_FieldDrawers.at(type)(name, value);
            ImGuiFieldDrawer::EndPropertyGrid();
            return result;
        }
        catch (std::exception exception) {
            return false;
        }
    }

    bool ImGuiFieldDrawer::DrawInt(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<int>();
        bool result = ImGui::DragInt(s_IDBuffer, value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawFloat(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<float>();
        bool result = ImGui::DragFloat(s_IDBuffer, value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawBool(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<bool>();
        bool result = ImGui::Checkbox(s_IDBuffer, value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec2(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec2>();
        bool result = ImGui::DragFloat2(s_IDBuffer, glm::value_ptr(*value));
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec3(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec3>();
        bool result = imgui::DrawVec3Control(s_IDBuffer, *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec4(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec4>();
        bool result = ImGui::ColorEdit4(s_IDBuffer, glm::value_ptr(*value));
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawTexture(std::string name, Reflect::Any& valueFrom)
    {
        ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                const char *path = (const char *) payload->Data;
                std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
                ref<Texture2D> texture = Texture2D::Create(texturePath.string());
                valueFrom = texture;
                return true;

                // if (texture->IsLoaded())
                // 	component.Sprite = texture;
                // else
                // 	FOX_CORE_WARN("Could not load texture %", texturePath.filename().string());
            }
            ImGui::EndDragDropTarget();
        }

        return false;
    }

//    bool ImGuiFieldDrawer::DrawEntityRef(std::string name, Reflect::Any& valueFrom)
//    {
//        Entity *en = valueFrom.TryCast<Entity>();
//        ImGui::Button(en != nullptr && *en ? en->GetName().c_str() : "Entity", ImVec2(100.0f, 0.0f));
//        if (ImGui::BeginDragDropTarget())
//        {
//            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SceneHierarchy")) {
//                Entity *entity = static_cast<Entity *>(payload->Data);
//                valueFrom = *entity;
//                return true;
//            }
//            ImGui::EndDragDropTarget();
//        }
//
//        return false;
//    }

    bool ImGuiFieldDrawer::DrawRigidbody2D_BodyType(std::string name, Reflect::Any& valueFrom)
    {
        auto value = *valueFrom.TryCast<Rigidbody2D::BodyType>();
        const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };

        int result = -1;
        if (DrawEnum(s_IDBuffer, bodyTypeString, 3, (int) value, result))
        {
            valueFrom = (Rigidbody2D::BodyType) result;
            return true;
        }
        return false;
    }

    bool ImGuiFieldDrawer::DrawEnum(std::string name, const char* typeString[], int size, int index, int& valueFrom)
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
}
