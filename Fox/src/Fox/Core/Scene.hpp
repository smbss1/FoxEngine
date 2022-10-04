/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Scene.hpp
*/

#ifndef FOX_SCENE_HPP_
#define FOX_SCENE_HPP_

#include "Core/UUID.hpp"
#include "Renderer/EditorCamera.hpp"

#include "entt.hpp"

namespace fox
{
    class Application;
    class Entity;
}

class b2World;

namespace fox
{
    class Scene
    {
    public:
        Scene(Application& app);
        ~Scene();

        static ref<Scene> Copy(ref<Scene> other);

        Entity NewEntity(const std::string& name = std::string());
        Entity NewEntityWithUUID(UUID uuid, const std::string &name);
        void DuplicateEntity(Entity entity);

        template<typename... Components>
        auto GetAllEntitiesWith()
        {
            return m_Registry.view<Components...>();
        }

        void DestroyEntity(Entity entity);

        void OnUpdateRuntime();
        void OnUpdateSimulation(EditorCamera& camera);
        void OnUpdateEditor(EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnSimulationStart();
        void OnSimulationStop();

        Entity GetPrimaryCameraEntity();

        Application& GetApp();
        Entity GetEntityByUUID(UUID uuid);

    private:

        void OnPhysics2DStart();
        void OnPhysics2DStop();

        void RenderScene();
        void RenderScene(EditorCamera& camera);

        template<typename T>
        void OnComponentAdded(Entity &e, T &component);

    private:
        Application& m_oApp;
        entt::registry m_Registry;
        b2World* m_PhysicsWorld = nullptr;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        std::unordered_map<UUID, entt::entity> m_EntityMap;

    protected:
        const float timeStep = 1.0f / 60.0f;
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };
}
#endif /* !SCENE_HPP_ */
