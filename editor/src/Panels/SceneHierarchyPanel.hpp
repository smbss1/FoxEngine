//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_SCENEHIERARCHYPANEL_HPP
#define FOXENGINE_SCENEHIERARCHYPANEL_HPP

#include "Scene/Scene.hpp"
#include "Ecs/Entity.hpp"
#include "Components/NativeScript.hpp"
#include "Panel.hpp"

namespace fox
{
    class OnContextChangeEvent;

    class SceneHierarchyPanel : public Panel
    {
    public:
        explicit SceneHierarchyPanel();
        ~SceneHierarchyPanel();

        void OnImGui() override;

        const Entity& GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(const Entity& e) { m_SelectedEntity = e; }

    private:
        void DrawEntityNode(Entity& e);
        void OnContextChangeChange(const OnContextChangeEvent& event);

    private:
        ref<Scene> m_pContext = nullptr;
        Entity m_SelectedEntity;
    };
}

#endif //FOXENGINE_SCENEHIERARCHYPANEL_HPP
