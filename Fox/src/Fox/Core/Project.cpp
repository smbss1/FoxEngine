//
// Created by samuel on 02/07/2021.
//

#include "Project.hpp"
#include "Asset/AssetManager.hpp"
#include "Core/ProjectSerializer.hpp"

namespace fox
{
    Ref<Project> Project::New()
    {
        s_ActiveProject = new_ref<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        FOX_INFO("Load Project: %", path);

        Ref<Project> project = new_ref<Project>();

        ProjectBeforeLoad.Invoke();

        ProjectSerializer serializer(project);
        if (serializer.Deserialize(path))
        {
            FOX_INFO("Load Successfully");

            project->m_ProjectDirectory = path.parent_path();
            s_ActiveProject = project;
            ProjectLoaded.Invoke();
            return s_ActiveProject;
        }

        return nullptr;
    }

    void Project::Unload()
    {
        if (!s_ActiveProject)
        {
            return;
        }

        ProjectBeforeUnload.Invoke();
        s_ActiveProject = nullptr;
        ProjectUnloaded.Invoke();
    }

    bool Project::SaveActive(const std::filesystem::path& path)
    {
        ProjectBeforeSave.Invoke();
        ProjectSerializer serializer(s_ActiveProject);
        if (serializer.Serialize(path))
        {
            s_ActiveProject->m_ProjectDirectory = path.parent_path();
            ProjectSaved.Invoke();
            return true;
        }

        return false;
    }
}
