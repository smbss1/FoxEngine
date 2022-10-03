//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_SCENEHIERARCHYPANEL_HPP
#define FOXENGINE_SCENEHIERARCHYPANEL_HPP

#include <Core/Scene.hpp>
#include <SharedLib.hpp>
#include "Ecs/Entity.hpp"
#include "Components/NativeScript.hpp"

namespace fox
{
    class SceneHierarchyPanel
    {
    public:
        explicit SceneHierarchyPanel();
        explicit SceneHierarchyPanel(const ref<Scene>& context);
        ~SceneHierarchyPanel();

        void SetContext(const ref<Scene>& context);

        void OnImGui();

        const Entity& GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(const Entity& e) { m_SelectedEntity = e; }

    private:
        void DrawEntityNode(Entity& e);
        void DrawComponents(Entity& entity);

        // void DrawComponent(const ref<Component> &component_ptr, Entity entity);

        // void DrawScripts(Entity entity, std::function<void(ScriptableBehaviour&)> callback);

        template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

    private:
        ref<Scene> m_pContext = nullptr;
        Entity m_SelectedEntity;

        scope<SharedLib> m_pScriptLib;
//        std::unordered_map<size_t, std::string> m_vScriptsNames;
//        std::unordered_map<size_t, ScriptCreator> m_vScripts;
    };
}

#endif //FOXENGINE_SCENEHIERARCHYPANEL_HPP
