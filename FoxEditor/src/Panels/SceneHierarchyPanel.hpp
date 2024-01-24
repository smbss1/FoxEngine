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

        void OnImGui(bool& isOpen) override;

        const Entity& GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(const Entity& e);

    private:
        void DrawEntityNode(const Entity& e);
        void OnContextChangeChange(const OnContextChangeEvent& event);

    private:
        Ref<Scene> m_pContext = nullptr;
        Entity m_SelectedEntity;
        bool m_bIsDeleted = false; // Is the entity deleted ?
        bool m_bIsDuplicated = false; // Is the entity deleted ?
    };
}

#endif //FOXENGINE_SCENEHIERARCHYPANEL_HPP
