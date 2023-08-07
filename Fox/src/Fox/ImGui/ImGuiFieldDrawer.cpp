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
#include "Core/Project.hpp"
#include "ImGuiExtensions.hpp"
#include "Asset/AssetManager.hpp"

namespace fox
{
    std::unordered_map<const Reflect::TypeDescriptor *, std::function<bool(std::string, Reflect::Any&)>> ImGuiFieldDrawer::m_Drawers = {
        {Reflect::Resolve<int>(),                       ImGuiFieldDrawer::DrawInt},
        {Reflect::Resolve<float>(),                     ImGuiFieldDrawer::DrawFloat},
        {Reflect::Resolve<bool>(),                      ImGuiFieldDrawer::DrawBool},
        {Reflect::Resolve<glm::vec2>(),                 ImGuiFieldDrawer::DrawVec2},
        {Reflect::Resolve<glm::vec3>(),                 ImGuiFieldDrawer::DrawVec3},
        {Reflect::Resolve<glm::vec4>(),                 ImGuiFieldDrawer::DrawVec4},
        {Reflect::Resolve<fox::Ref<fox::Texture2D>>(),  ImGuiFieldDrawer::DrawTexture},
//        {Reflect::Resolve<AssetHandle>(),               ImGuiFieldDrawer::DrawAsset},

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

    bool ImGuiFieldDrawer::Draw(const Reflect::TypeDescriptor *type, std::string name, Reflect::Any& value)
    {
        try {
            bool result = m_Drawers.at(type)(name, value);
            return result;
        }
        catch (std::exception& exception) {
            return false;
        }
    }

    bool ImGuiFieldDrawer::DrawField(ScriptFieldType type, std::string name, Reflect::Any& value)
    {
        try {
            bool result = m_FieldDrawers.at(type)(name, value);
            return result;
        }
        catch (std::exception exception) {
            return false;
        }
    }

    bool ImGuiFieldDrawer::DrawInt(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<int>();
        bool result = UI::Property(name, *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawFloat(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<float>();
        bool result = UI::Property(name, *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawBool(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<bool>();
        bool result = UI::Property(name, *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec2(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec2>();
        bool result = UI::Property(name, *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec3(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec3>();
        bool result = UI::DragScalar(name, *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawVec4(std::string name, Reflect::Any& valueFrom)
    {
        auto *value = valueFrom.TryCast<glm::vec4>();
        bool result = UI::Property(name, *value);
        valueFrom = *value;
        return result;
    }

    bool ImGuiFieldDrawer::DrawTexture(std::string name, Reflect::Any& valueFrom)
    {
        AssetHandle& value = *valueFrom.TryCast<AssetHandle>();
        return UI::AssetField(name, value, true, {".png"});
//        if (ImGui::BeginDragDropTarget())
//        {
//            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
//                const char *path = (const char *) payload->Data;
//                std::filesystem::path texturePath = Project::AssetsDir() / path;
//                Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
//                valueFrom = texture;
//                return true;
//
//                // if (texture->IsLoaded())
//                // 	component.Sprite = texture;
//                // else
//                // 	FOX_CORE_WARN("Could not load texture %", texturePath.filename().string());
//            }
//            ImGui::EndDragDropTarget();
//        }

//        return false;
    }

//    bool ImGuiFieldDrawer::DrawAsset(std::string name, Reflect::Any& valueFrom)
//    {
//        auto *value = valueFrom.TryCast<AssetHandle>();
//        Ref<Asset> asset = AssetManager::GetAsset<Asset>(*value);
//        if (asset == nullptr)
//            return false;
//        switch (asset->GetAssetType())
//        {
////            case AssetType::Texture:
////                return DrawTexture(name, valueFrom);
//
//            default:
//                break;
//        }
//
////        valueFrom = *value;
//        return false;
//    }


//    bool ImGuiFieldDrawer::DrawEntityRef(std::string name, Reflect::Any& valueFrom)
//    {
//        Entity *en = valueFrom.TryCast<Entity>();
//        ImGui::Button(en != nullptr && *en ? en->GetName().c_str() : "Entity", ImVec2(100.0f, 0.0f));
//        if (ImGui::BeginDragDropTarget())
//        {
//            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SceneHierarchy")) {
//                size_t count = payload->DataSize / sizeof(UUID);
//
//                for (size_t i = 0; i < count; i++)
//                {
//                    UUID droppedEntityID = *(((UUID*)payload->Data) + i);
//                    Entity droppedEntity = m_pContext->GetEntityByUUID(droppedEntityID);
//                }
//
//                //Entity *entity = static_cast<Entity *>(payload->Data);
//                //valueFrom = *entity;
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
        if (UI::Property(name, bodyTypeString, 3, (int) value, result))
        {
            valueFrom = (Rigidbody2D::BodyType) result;
            return true;
        }
        return false;
    }
}
