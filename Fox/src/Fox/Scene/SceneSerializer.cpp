//
// Created by samuel on 30/06/2021.
//

#include "Core/Assert.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "Core/Application.hpp"
#include "Components/ScriptableBehaviour.hpp"
#include "Components.hpp"
#include "SceneSerializer.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "EntitySerializer.hpp"

namespace fox
{
    SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_pScene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        FOX_ASSERT(entity.has<IDComponent>())
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

        // Serialize components (except IDComponent)
		EntitySerializer::SerializeAllEntityComponents(out,entity);

        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string &filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_pScene->m_Registry.each([&](auto entityID)
         {
             Entity entity = { entityID, m_pScene.Raw() };
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
        FOX_CORE_INFO("Deserializing scene '%'", sceneName);

        auto entities = data["Entities"];
        if (!entities)
            return true;
        for (auto entity : entities)
        {
            uint64_t uuid = entity["Entity"].as<uint64_t>();

            std::string name;
            auto nameComponent = entity["NameComponent"];
            if (nameComponent)
                name = nameComponent["Name"].as<std::string>();

            FOX_CORE_INFO("Deserialized entity with ID = %, name = %", uuid, name);

            Entity deserializedEntity = m_pScene->NewEntityWithUUID(uuid, name);

            // Deserialize components (except IDComponent and TagComponent)
            EntitySerializer::DeserializeAllEntryComponents(entity, deserializedEntity);
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
    {
        FOX_CORE_ASSERT(false, "");
        return false;
    }
}
