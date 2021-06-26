//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_SCENEHIERARCHYPANEL_HPP
#define FOXENGINE_SCENEHIERARCHYPANEL_HPP

#include <Core/Scene.hpp>

namespace fox
{
    class SceneHierarchyPanel
    {
    public:
        explicit SceneHierarchyPanel() = default;
        explicit SceneHierarchyPanel(World* context);

        void SetContext(World* context);

        void OnImGui();

    private:
        void DrawEntityNode(Entity& e);
        void DrawComponents(Entity& entity);

    private:
        World* m_pContext = nullptr;
        Entity m_SelectedEntity;
    };
}

#endif //FOXENGINE_SCENEHIERARCHYPANEL_HPP
