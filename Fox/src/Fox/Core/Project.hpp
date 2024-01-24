//
// Created by samuel on 02/07/2021.
//

#ifndef FOXENGINE_FPATHS_HPP
#define FOXENGINE_FPATHS_HPP

#include <string>
#include "Core/Base.hpp"
#include "Ref.hpp"
#include "Assert.hpp"
#include "FoxSignalDefs.hpp"

namespace fox
{
    struct ProjectConfig
    {
        std::string Name = "";

        std::string AssetDirectory = "";
        std::string AssetRegistryPath = "";

        std::string ScriptModulePath = "";
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
        inline static ProjectEvent ProjectBeforeLoad;
        inline static ProjectEvent ProjectLoaded;
        inline static ProjectEvent ProjectBeforeUnload;
        inline static ProjectEvent ProjectUnloaded;
        inline static ProjectEvent ProjectBeforeSave;
        inline static ProjectEvent ProjectSaved;

        const ProjectConfig& GetConfig() const { return m_Config; }

        static Ref<Project> GetActive() { return s_ActiveProject; }
        static Ref<Project> Load(const std::filesystem::path& path);
        static void Unload();
        static Ref<Project> New();
        static bool SaveActive(const std::filesystem::path& path);

        static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return AssetsDir() / path; }

        static std::filesystem::path Project::GetAssetAbsolutePath(const std::filesystem::path& path)
        {
            return AssetsDir() / path;
        }

        static const std::string& ProjectName()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->GetConfig().Name;
        }

        static fs::path ProjectDir()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->m_ProjectDirectory;
        }

        static fs::path AssetsDir()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return fs::path(s_ActiveProject->m_ProjectDirectory) / s_ActiveProject->GetConfig().AssetDirectory;
        }

        static fs::path AssetRegistryPath()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return fs::path(s_ActiveProject->m_ProjectDirectory) / s_ActiveProject->GetConfig().AssetRegistryPath;
        }

        static fs::path ScriptModulePath()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return fs::path(s_ActiveProject->m_ProjectDirectory) / s_ActiveProject->GetConfig().ScriptModulePath;
        }

        static fs::path ScriptModuleFilePath()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return ScriptModulePath() / fox::format("%.dll", ProjectName());
        }

        static fs::path CacheDirectory()
        {
            FOX_CORE_ASSERT(s_ActiveProject);
            return fs::path(s_ActiveProject->m_ProjectDirectory) / "Cache";
        }

    private:
        ProjectConfig m_Config {};
        std::filesystem::path m_ProjectDirectory;

        friend class ProjectSerializer;

        inline static Ref<Project> s_ActiveProject;

    };
}

#endif //FOXENGINE_FPATHS_HPP
