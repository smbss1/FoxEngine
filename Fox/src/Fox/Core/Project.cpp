//
// Created by samuel on 02/07/2021.
//

#include "Project.hpp"
#include "Asset/AssetManager.hpp"

namespace fox
{
    void Project::SetActive(Ref<Project> project)
    {
        if (s_ActiveProject)
        {
            AssetManager::Shutdown();
        }

        s_ActiveProject = project;
        if (s_ActiveProject)
        {
            AssetManager::Init();
        }
    }
}
