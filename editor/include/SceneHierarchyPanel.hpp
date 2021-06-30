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
        explicit SceneHierarchyPanel(const ref<Scene>& context);

        void SetContext(const ref<Scene>& context);

        void OnImGui();

    private:
        void DrawEntityNode(Entity& e);
        void DrawComponents(Entity& entity);

        template<typename T>
        void DrawComponent(const std::string& name, Entity entity, std::function<void(T& component)> callback);

    private:
        ref<Scene> m_pContext = nullptr;
        Entity m_SelectedEntity;
    };
}

#endif //FOXENGINE_SCENEHIERARCHYPANEL_HPP
