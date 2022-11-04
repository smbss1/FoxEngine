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
#include "Core/Timestep.hpp"

#include "entt.hpp"

namespace fox
{
    class Application;
    class Entity;
}

class b2World;

namespace fox
{
    class TransformComponent;
    class Prefab;
    class Scene : public RefCounted
    {
    public:
        Scene(bool initalize = true);
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity NewEntity(const std::string& name = std::string());
        Entity NewChildEntity(Entity parent, const std::string& name = std::string());
        Entity NewEntityWithUUID(UUID uuid, const std::string &name);
        Entity GetEntityByUUID(UUID uuid);
        Entity TryGetEntityByUUID(UUID uuid);
        Entity FindEntityByName(std::string_view name);
        void ParentEntity(Entity entity, Entity parent);
        void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

        const std::unordered_map<UUID, entt::entity>& GetEntities() const;


        void ConvertToLocalSpace(Entity entity);
        void ConvertToWorldSpace(Entity entity);
        glm::mat4 GetWorldSpaceTransformMatrix(Entity entity);
        TransformComponent GetWorldSpaceTransform(Entity entity);


        void DuplicateEntity(Entity entity);
        Entity CloneEntity(Entity entity, Entity parent, const glm::vec3* translation = nullptr, const glm::vec3* rotation = nullptr, const glm::vec3* scale = nullptr);
        Entity Instantiate(Ref<Prefab> prefab, const glm::vec3* translation = nullptr, const glm::vec3* rotation = nullptr, const glm::vec3* scale = nullptr);

        template<typename... Components>
        auto GetAllEntitiesWith()
        {
            return m_Registry.view<Components...>();
        }

        void SubmitToDestroyEntity(Entity entity);
        void DestroyEntity(Entity entity, bool excludeChildren = false, bool first = true);

        template<typename Fn>
        void SubmitPostUpdateFunc(Fn&& func)
        {
            m_PostUpdateQueue.emplace_back(func);
        }

        void OnUpdateRuntime(Timestep ts);
        void OnUpdateSimulation(EditorCamera& camera, Timestep ts);
        void OnUpdateEditor(EditorCamera& camera, Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnSimulationStart();
        void OnSimulationStop();

        Entity GetPrimaryCameraEntity();

        bool IsRunning() const { return m_IsRunning; }
        void CopyAllComponentsIfExists(Entity dst, Entity src);

    private:

        void ProcessPostQueue();

        void OnPhysics2DStart();
        void OnPhysics2DStop();

        void RenderScene();
        void RenderScene(EditorCamera& camera);

        void SortEntities();

        static Ref<Scene> CreateEmpty();

        void OnCameraComponentConstruct(entt::registry& registry, entt::entity entity);
        void OnRigidBody2DComponentConstruct(entt::registry& registry, entt::entity entity);
        void OnRigidBody2DComponentDestroy(entt::registry& registry, entt::entity entity);
        void OnBoxCollider2DComponentConstruct(entt::registry& registry, entt::entity entity);
        void OnBoxCollider2DComponentDestroy(entt::registry& registry, entt::entity entity);
        void OnCircleCollider2DComponentConstruct(entt::registry& registry, entt::entity entity);
        void OnCircleCollider2DComponentDestroy(entt::registry& registry, entt::entity entity);

    private:
        bool m_IsRunning = false;
        Application& m_oApp;
        entt::registry m_Registry;
        b2World* m_PhysicsWorld = nullptr;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        std::unordered_map<UUID, entt::entity> m_EntityMap;

        std::vector<std::function<void()>> m_PostUpdateQueue;

    protected:
        const float timeStep = 1.0f / 60.0f;
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
        friend class EntitySerializer;
        friend class PrefabSerializer;
        friend class Prefab;
    };
}
#endif /* FOX_SCENE_HPP_ */
