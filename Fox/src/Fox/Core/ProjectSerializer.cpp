//
// Created by samuel on 22/10/22.
//

#include "ProjectSerializer.hpp"
#include "yaml-cpp/yaml.h"
#include "Physics2D.hpp"

#include <filesystem>
#include <fstream>

namespace fox
{
    ProjectSerializer::ProjectSerializer(Ref<Project> project)
        : m_Project(project)
    {
    }

    void ProjectSerializer::Serialize(const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Project" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << m_Project->m_Config.Name;
            out << YAML::Key << "AssetDirectory" << YAML::Value << m_Project->m_Config.AssetDirectory;
            out << YAML::Key << "AssetRegistry" << YAML::Value << m_Project->m_Config.AssetRegistryPath;
            out << YAML::Key << "ScriptModulePath" << YAML::Value << m_Project->m_Config.ScriptModulePath;
            out << YAML::Key << "StartScene" << YAML::Value << m_Project->m_Config.StartScene;
            out << YAML::Key << "ReloadAssemblyOnPlay" << YAML::Value << m_Project->m_Config.ReloadAssemblyOnPlay;
            out << YAML::Key << "AutoSave" << YAML::Value << m_Project->m_Config.EnableAutoSave;
            out << YAML::Key << "AutoSaveInterval" << YAML::Value << m_Project->m_Config.AutoSaveIntervalSeconds;

            out << YAML::Key << "Physics2D" << YAML::Value;
            {
                out << YAML::BeginMap;

                // > 1 because of the Default layer
                if (Physics2D::GetLayerCount() > 1)
                {
                    out << YAML::Key << "Layers";
                    out << YAML::Value << YAML::BeginSeq;
                    for (const auto& layer : Physics2D::GetLayers())
                    {
                        // Never serialize the Default layer
                        if (layer.LayerID == 0)
                            continue;

                        out << YAML::BeginMap;
                        out << YAML::Key << "Name" << YAML::Value << layer.Name;
//                        out << YAML::Key << "CollidesWithSelf" << YAML::Value << layer.Collisions;
                        out << YAML::Key << "CollidesWith" << YAML::Value;
                        out << YAML::BeginSeq;
                        auto collisions = Physics2D::GetCollisions(layer.LayerID);
                        for (const auto& collidingLayer : collisions)
                        {
                            out << YAML::BeginMap;
                            out << YAML::Key << "Name" << YAML::Value << collidingLayer.Name;
                            out << YAML::EndMap;
                        }
                        out << YAML::EndSeq;
                        out << YAML::EndMap;
                    }
                    out << YAML::EndSeq;
                }

                out << YAML::EndMap;
            }

//            out << YAML::Key << "Log" << YAML::Value;
//            {
//                out << YAML::BeginMap;
//                auto& tags = Log::EnabledTags();
//                for (auto& [name, details] : tags)
//                {
//                    if (!name.empty()) // Don't serialize untagged log
//                    {
//                        out << YAML::Key << name << YAML::Value;
//                        out << YAML::BeginMap;
//                        {
//                            out << YAML::Key << "Enabled" << YAML::Value << details.Enabled;
//                            out << YAML::Key << "LevelFilter" << YAML::Value << Log::LevelToString(details.LevelFilter);
//                            out << YAML::EndMap;
//                        }
//
//                    }
//                }
//                out << YAML::EndMap;
//            }

//            out << YAML::EndMap;
        }
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
    {
        Physics2D::Init();

        std::ifstream stream(filepath);
        FOX_CORE_ASSERT(stream);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Project"])
            return false;

        YAML::Node rootNode = data["Project"];
        if (!rootNode["Name"])
            return false;

        auto& config = m_Project->m_Config;
        config.Name = rootNode["Name"].as<std::string>();

        config.AssetDirectory = rootNode["AssetDirectory"].as<std::string>();
        config.AssetRegistryPath = rootNode["AssetRegistry"].as<std::string>();

        config.ScriptModulePath = rootNode["ScriptModulePath"].as<std::string>();
        config.DefaultNamespace = rootNode["DefaultNamespace"] ? rootNode["DefaultNamespace"].as<std::string>() : config.Name;
        config.StartScene = rootNode["StartScene"] ? rootNode["StartScene"].as<std::string>() : "";

        config.ReloadAssemblyOnPlay = rootNode["ReloadAssemblyOnPlay"] ? rootNode["ReloadAssemblyOnPlay"].as<bool>() : true;

        config.EnableAutoSave = rootNode["AutoSave"].as<bool>(false);
        config.AutoSaveIntervalSeconds = rootNode["AutoSaveInterval"].as<int>(300);

        std::filesystem::path projectPath = filepath;
        config.ProjectFileName = projectPath.filename().string();
        config.ProjectDirectory = projectPath.parent_path().string();

        // Physics
        auto physicsNode = rootNode["Physics2D"];
        if (physicsNode)
        {
            auto physicsLayers = physicsNode["Layers"];
            if (physicsLayers)
            {
                for (auto layer : physicsLayers)
                {
                    Physics2D::AddLayer(layer["Name"].as<std::string>(), false);
                }

                for (auto layer : physicsLayers)
                {
                    PhysicLayer* layerInfo = nullptr;
                    Physics2D::GetLayer(layer["Name"].as<std::string>(), layerInfo);

                    auto collidesWith = layer["CollidesWith"];
                    if (collidesWith)
                    {
                        for (auto collisionLayer : collidesWith)
                        {
                            PhysicLayer* otherLayer = nullptr;
                            if (Physics2D::GetLayer(collisionLayer["Name"].as<std::string>(), otherLayer))
                            {
                                Physics2D::SetCollision(layerInfo->LayerID, otherLayer->LayerID, true);
                            }
                        }
                    }
                }
            }
        }

//        // Log
//        auto logNode = rootNode["Log"];
//        if (logNode)
//        {
//            auto& tags = Log::EnabledTags();
//            for (auto node : logNode)
//            {
//                std::string name = node.first.as<std::string>();
//                auto& details = tags[name];
//                details.Enabled = node.second["Enabled"].as<bool>();
//                details.LevelFilter = Log::LevelFromString(node.second["LevelFilter"].as<std::string>());
//            }
//        }
        return true;
    }

}
