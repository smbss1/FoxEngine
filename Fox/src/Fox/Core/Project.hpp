//
// Created by samuel on 02/07/2021.
//

#ifndef FOXENGINE_FPATHS_HPP
#define FOXENGINE_FPATHS_HPP

#include <string>
#include <filesystem>
#include "Ref.hpp"
#include "Assert.hpp"

namespace fox
{
    struct ProjectConfig
    {
        std::string Name;

        std::string AssetDirectory;
        std::string AssetRegistryPath;

        std::string ScriptModulePath;
        std::string DefaultNamespace;

        std::string StartScene;

        bool ReloadAssemblyOnPlay;

        bool EnableAutoSave = false;
        int AutoSaveIntervalSeconds = 300;

        // Not serialized
        std::string ProjectFileName;
        std::string ProjectDirectory;
    };

    class Project : public RefCounted
    {
    public:
        const ProjectConfig& GetConfig() const { return m_Config; }

        static Ref<Project> GetActive() { return s_ActiveProject; }
        static void SetActive(Ref<Project> project);

        static const std::string& ProjectName()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->GetConfig().Name;
        }

        static std::filesystem::path ProjectDir()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->GetConfig().ProjectDirectory;
        }

        static std::filesystem::path AssetsDir()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetDirectory;
        }

        static std::filesystem::path AssetRegistryPath()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetRegistryPath;
        }

        static std::filesystem::path ScriptModulePath()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().ScriptModulePath;
        }

        static std::filesystem::path ScriptModuleFilePath()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return ScriptModulePath() / fox::format("%.dll", ProjectName());
        }

        static std::filesystem::path CacheDirectory()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / "Cache";
        }

    private:
        ProjectConfig m_Config;

        friend class ProjectSerializer;

        inline static Ref<Project> s_ActiveProject;

    };
}

#endif //FOXENGINE_FPATHS_HPP
