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
        {Reflect::Resolve<int>(),                    ImGuiFieldDrawer::DrawInt},
        {Reflect::Resolve<float>(),                    ImGuiFieldDrawer::DrawFloat},
        {Reflect::Resolve<bool>(),                    ImGuiFieldDrawer::DrawBool},
        {Reflect::Resolve<glm::vec2>(),                ImGuiFieldDrawer::DrawVec2},
        {Reflect::Resolve<glm::vec3>(),                ImGuiFieldDrawer::DrawVec3},
        {Reflect::Resolve<glm::vec4>(),                ImGuiFieldDrawer::DrawVec4},
        {Reflect::Resolve<fox::ref<fox::Texture2D>>(), ImGuiFieldDrawer::DrawTexture},

        {Reflect::Resolve<Rigidbody2D::BodyType>(),    ImGuiFieldDrawer::DrawRigidbody2D_BodyType},
    };

    bool ImGuiFieldDrawer::Draw(const Reflect::TypeDescriptor *type, std::string name, Reflect::Any& value)
    {
        try {
            return m_Drawers.at(type)(name, value);
        }
        catch (std::exception exception) {
            return false;
        }
    }

//    void ImGuiFieldDrawer::DrawField(ScriptFieldInstance& scriptField, std::string name, Reflect::Any& value)
//    {
////        const Reflect::TypeDescriptor *reflectType = Reflect::Resolve(Utils::ScriptFieldTypeToString(type));
////        return Draw(reflectType, name, value);
//
//        Reflect::Any data = Reflect::Any(scriptField.GetValue<float>());
//        if (ImGuiFieldDrawer::Draw(field.Type, name, data))
//            Reflect::Resolve<ScriptFieldInstance>()->GetDataMember("Value")->Set(scriptField, data);
//    }

    bool ImGuiFieldDrawer::DrawInt(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<int>();
        bool result = ImGui::DragInt(name.c_str(), value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawFloat(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<float>();
        bool result = ImGui::DragFloat(name.c_str(), value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawBool(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<bool>();
        bool result = ImGui::Checkbox(name.c_str(), value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec2(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec2>();
        bool result = ImGui::DragFloat2(name.c_str(), glm::value_ptr(*value));
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec3(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec3>();
        bool result = imgui::DrawVec3Control(name.c_str(), *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec4(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec4>();
        bool result = ImGui::ColorEdit4(name.c_str(), glm::value_ptr(*value));
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawTexture(std::string name, Reflect::Any& valueFrom)
    {
        ImGui::Button(name.c_str(), ImVec2(100.0f, 0.0f));
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
                // 	fox::warn("Could not load texture %", texturePath.filename().string());
            }
            ImGui::EndDragDropTarget();
        }

        return false;
    }

    bool ImGuiFieldDrawer::DrawRigidbody2D_BodyType(std::string name, Reflect::Any& valueFrom)
    {
        auto value = *valueFrom.TryCast<Rigidbody2D::BodyType>();
        const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };

        int result = -1;
        if (DrawEnum(name, bodyTypeString, 3, (int) value, result))
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
