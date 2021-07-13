//
// Created by samuel on 30/06/2021.
//

#include <Core/Assert.hpp>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Core/Application.hpp>
#include <Components/EntityName.hpp>
#include <Components/Transform.hpp>
#include <Components/CameraComponent.hpp>
#include <Components/SpriteRenderer.hpp>
#include <Components/NativeScript.hpp>
#include "Core/SceneSerializer.hpp"


namespace YAML {

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

}

namespace fox
{
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(const ref<Scene> &scene) : m_pScene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.get_id(); // TODO: Entity ID goes here

        auto oNameComponent = entity.get<EntityName>();
        if (oNameComponent)
        {
            out << YAML::Key << "NameComponent";
            out << YAML::BeginMap; // NameComponent

            auto& name = oNameComponent->name;
            out << YAML::Key << "Name" << YAML::Value << name;

            out << YAML::EndMap; // NameComponent
        }

        auto oTransform = entity.get<TransformComponent>();
        if (oTransform)
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            auto& tc = *oTransform;
            out << YAML::Key << "Position" << YAML::Value << tc.position;
            out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.scale;

            out << YAML::EndMap; // TransformComponent
        }

        auto oCamera = entity.get<CameraComponent>();
        if (oCamera)
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& camera = oCamera->camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
            out << YAML::EndMap; // Camera

            out << YAML::Key << "Primary" << YAML::Value << oCamera->Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << oCamera->FixedAspectRatio;

            out << YAML::EndMap; // CameraComponent
        }

        auto oSpriteRenderer = entity.get<SpriteRenderer>();
        if (oSpriteRenderer)
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap; // SpriteRendererComponent

            auto& spriteRendererComponent = *oSpriteRenderer;
            out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color.get();

            if (spriteRendererComponent.Sprite.get())
                out << YAML::Key << "Sprite" << YAML::Value << spriteRendererComponent.Sprite.get()->GetId();
            else
                out << YAML::Key << "Sprite" << YAML::Value << "";

            out << YAML::EndMap; // SpriteRendererComponent
        }

        auto oNativeScript = entity.get<NativeScript>();
        if (oNativeScript)
        {
            out << YAML::Key << "NativeScriptComponent";
            out << YAML::BeginMap; // NativeScriptComponent
            for (auto& script : oNativeScript->m_vScripts)
            {
                out << YAML::Key << "Script" << YAML::Value << script.first;

                // TODO: Save variables from the script
                // out << YAML::BeginMap; // Script

                // out << YAML::Key << "ID" <<

                // out << YAML::EndMap; // Script
            }
            out << YAML::EndMap; // NativeScriptComponent
        }

        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string &filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_pScene->GetWorld().each([&](auto entityID)
         {
             Entity entity = { &m_pScene->GetWorld(), entityID };
             if (!entity)
                 return;

             SerializeEntity(out, entity);
         });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    void SceneSerializer::SerializeRuntime(const std::string &filepath)
    {
        FOX_CORE_ASSERT(false, "");
    }

    bool SceneSerializer::Deserialize(const std::string &filepath)
    {
        YAML::Node data = YAML::LoadFile(filepath);
        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        fox::info("Deserializing scene '%'", sceneName);

        auto entities = data["Entities"];
        if (!entities)
            return true;
        for (auto entity : entities)
        {
            EntityId uuid = entity["Entity"].as<EntityId>(); // TODO

            std::string name;
            auto nameComponent = entity["NameComponent"];
            if (nameComponent)
                name = nameComponent["Name"].as<std::string>();

            fox::info("Deserialized entity with ID = %, name = %", uuid, name);

            Entity deserializedEntity = m_pScene->NewEntity(name);

            auto transformComponent = entity["TransformComponent"];
            if (transformComponent)
            {
                // Entities always have transforms
                auto& tc = *deserializedEntity.get<TransformComponent>();
                tc.position = transformComponent["Position"].as<glm::vec3>();
                tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
                tc.scale = transformComponent["Scale"].as<glm::vec3>();
            }

            auto cameraComponent = entity["CameraComponent"];
            if (cameraComponent)
            {
                auto& cc = deserializedEntity.add<CameraComponent>();

                auto cameraProps = cameraComponent["Camera"];
                cc.camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                cc.camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                cc.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                cc.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                cc.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                cc.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

                cc.Primary = cameraComponent["Primary"].as<bool>();
                cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
            }

            auto spriteRendererComponent = entity["SpriteRendererComponent"];
            if (spriteRendererComponent)
            {
                auto& src = deserializedEntity.add<SpriteRenderer>();
                src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
                auto strFilepath = spriteRendererComponent["Sprite"].as<std::string>();
                if (!strFilepath.empty()) {
                    src.Sprite = Texture2D::Create(strFilepath);
                }
            }

            auto oNativeScriptComponent = entity["NativeScriptComponent"];
            if (oNativeScriptComponent)
            {
                auto& src = deserializedEntity.add<NativeScript>();

                for (auto script : oNativeScriptComponent)
                {
                    auto tag = script.second.as<std::size_t>();
                    ScriptCreator func = m_pScene->GetApp().GetScripts()[tag];
                    src.add(tag, func());
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
    {
        FOX_CORE_ASSERT(false, "");
        return false;
    }
}