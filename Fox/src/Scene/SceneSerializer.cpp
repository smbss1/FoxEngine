//
// Created by samuel on 30/06/2021.
//

#include <Core/Assert.hpp>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Core/Application.hpp>
#include <Components.hpp>
#include <Serialization/ReflectionToJson.hpp>
#include <Serialization/ReflectionFromJson.hpp>
#include "Scene/SceneSerializer.hpp"

namespace fox
{
    SceneSerializer::SceneSerializer(const ref<Scene> &scene) : m_pScene(scene)
    {
    }

    static void SerializeEntity(fox::json::Value& out, Entity entity)
    {
        // out["ID"] = entity.get_id();
        json::Value& vJsonComponents = out["Components"];

        auto vComponents = entity.GetAll();
        int i = 0;
        for (auto component : vComponents)
        {
            rttr::instance obj = *component;

            // If we have the Meta 'NO_SERIALIZE' so don't serialize this component
            if (obj.get_derived_type().get_metadata("NO_SERIALIZE"))
                continue;
            vJsonComponents[i]["Type"] = obj.get_derived_type().get_name().data();
            serialization::to_json_recursively(obj, vJsonComponents[i][obj.get_derived_type().get_name().data()]);
            i++;
        }

//        auto oNativeScript = entity.get<NativeScript>();
//        if (oNativeScript)
//        {
//            out << YAML::Key << "NativeScriptComponent";
//            out << YAML::BeginMap; // NativeScriptComponent
//            for (auto& script : oNativeScript->m_vScripts)
//            {
//                out << YAML::Key << "Script" << YAML::Value << script.first;
//
//                // TODO: Save variables from the script
//                // out << YAML::BeginMap; // Script
//
//                // out << YAML::Key << "ID" <<
//
//                // out << YAML::EndMap; // Script
//            }
//            out << YAML::EndMap; // NativeScriptComponent
//        }
    }

    void SceneSerializer::Serialize(const std::string &filepath)
    {
        fox::json::Value out;
        out["Scene"] = "Untitled";
        json::Value& entities = out["Entities"];
        int i = 0;
        m_pScene->GetWorld().each([&](auto entityID)
         {
             Entity entity = { &m_pScene->GetWorld(), entityID };
             if (!entity)
                 return;

             SerializeEntity(entities[i], entity);
             ++i;
         });

        std::ofstream fout(filepath);
        fout << out;
    }

    void SceneSerializer::SerializeRuntime(const std::string &filepath)
    {
        FOX_CORE_ASSERT(false, "");
    }

    bool SceneSerializer::Deserialize(const std::string &filepath)
    {
        std::string out;
        if (!file::ReadFile(filepath, out))
            return false;
        json::Value data = json::parse(out);
        if (data["Scene"].is_null())
            return false;

        std::string sceneName = data["Scene"].get<std::string>();
        fox::info("Deserializing scene '%'", sceneName);

        auto& entities = data["Entities"];
        if (entities.is_null())
            return true;

        json::Array array = entities.get<json::Array>();
        for (std::size_t i = 0; i < array.size(); ++i) {
            fox::Entity ent = m_pScene->NewEntity("");
            auto& oJsonEntity = entities[i];
            auto& oJsonComponents = oJsonEntity["Components"];

            if (!oJsonEntity["ID"].is_null())
                ent.set_id(oJsonEntity["ID"].get<EntityId>());
            json::Array comp_array = oJsonComponents.get<json::Array>();
            for (std::size_t j = 0; j < comp_array.size(); ++j)
            {
                json::Object comp_obj = oJsonComponents[j].get<json::Object>();
                std::string strTypeName = comp_obj["Type"].get<std::string>();

                rttr::variant var = rttr::type::get_by_name(strTypeName).create();
                auto component = var.get_value<ref<Component>>();
                serialization::from_json_recursively(component, comp_obj[strTypeName]);

                ent.set(component);
            }

            fox::info("Deserialized entity with ID = %, name = %", ent.get_id(), ent.get<EntityName>()->name);
            fox::info("Size: %", ent.GetAll().size());
        }

//            auto cameraComponent = entity["CameraComponent"];
//            if (cameraComponent)
//            {
//                auto& cc = deserializedEntity.add<CameraComponent>();
//
//                auto cameraProps = cameraComponent["Camera"];
//                cc.camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
//
//                cc.camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
//                cc.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
//                cc.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
//
//                cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
//                cc.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
//                cc.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
//
//                cc.Primary = cameraComponent["Primary"].as<bool>();
//                cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
//            }
//
//            auto spriteRendererComponent = entity["SpriteRendererComponent"];
//            if (spriteRendererComponent)
//            {
//                auto& src = deserializedEntity.add<SpriteRenderer>();
//                src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
//                auto strFilepath = spriteRendererComponent["Sprite"].as<std::string>();
//                if (!strFilepath.empty()) {
//                    src.Sprite = Texture2D::Create(strFilepath);
//                }
//            }
//
//            auto oNativeScriptComponent = entity["NativeScriptComponent"];
//            if (oNativeScriptComponent)
//            {
//                auto& src = deserializedEntity.add<NativeScript>();
//
//                for (auto script : oNativeScriptComponent)
//                {
//                    auto tag = script.second.as<std::size_t>();
//                    ScriptCreator func = m_pScene->GetApp().GetScripts()[tag];
//                    src.add(tag, func());
//                }
//            }
//        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
    {
        FOX_CORE_ASSERT(false, "");
        return false;
    }
}