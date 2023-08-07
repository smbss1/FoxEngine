/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Scene.cpp
*/

#include "Scene.hpp"
#include "Renderer/Camera.hpp"
#include "Components/CameraComponent.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Core/Application.hpp"
#include "Components/ScriptableBehaviour.hpp"
#include "Components/NativeScript.hpp"
#include "Components/NameComponent.hpp"
#include "Components.hpp"
#include "Ecs/Entity.hpp"
#include "Prefab.hpp"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_contact.h"
#include "Core/Assert.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Asset/AssetManager.hpp"
#include "Renderer/Renderer.hpp"
#include "Physics2D.hpp"

namespace fox
{
    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2D::BodyType bodyType)
    {
        switch (bodyType)
        {
            case Rigidbody2D::BodyType::Static:    return b2_staticBody;
            case Rigidbody2D::BodyType::Dynamic:   return b2_dynamicBody;
            case Rigidbody2D::BodyType::Kinematic: return b2_kinematicBody;
        }

        FOX_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }

    class PhysicsContactListener2D : public b2ContactListener
    {
    public:
        Scene* m_Scene;

        void BeginContact(b2Contact* contact) final override
        {
            b2Fixture* a = contact->GetFixtureA();
            b2Fixture* b = contact->GetFixtureB();

            bool aSensor = a->IsSensor();
            bool bSensor = b->IsSensor();

            Entity e1 = { (entt::entity)(uint32_t)a->GetBody()->GetUserData().pointer, m_Scene };
            Entity e2 = { (entt::entity)(uint32_t)b->GetBody()->GetUserData().pointer, m_Scene };

//            if (e1.has<BuoyancyEffector2DComponent>() && aSensor
//                && !e2.HasComponent<BuoyancyEffector2DComponent>() && b->GetBody()->GetType() == b2_dynamicBody)
//            {
//                m_BuoyancyFixtures.insert(eastl::make_pair(a, b));
//            }
//            else if (e2.HasComponent<BuoyancyEffector2DComponent>() && bSensor
//                     && !e1.HasComponent<BuoyancyEffector2DComponent>() && a->GetBody()->GetType() == b2_dynamicBody)
//            {
//                m_BuoyancyFixtures.insert(eastl::make_pair(b, a));
//            }

            b2WorldManifold worldManifold;
            contact->GetWorldManifold(&worldManifold);
//            b2Vec2 point = worldManifold.points[0];
//            b2Vec2 velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
//            b2Vec2 velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            Collision2DData collisionData;
            collisionData.OtherEntityID = e2.GetUUID();
//            collisionData.RelativeVelocity = { velocityB.x - velocityA.x, velocityB.y - velocityA.y };

            if (e1.has<ScriptComponent>())
            {
                const auto& sc = e1.get<ScriptComponent>();
                if (!bSensor)
                {
                    ScriptEngine::GetEntityScriptInstance(e1.GetUUID())->InvokeOnCollisionEnter2D(collisionData);
//                    for (const auto& className : sc.Classes)
//                        ScriptEngine::GetInstance(e1, className)->InvokeOnCollisionEnter2D(collisionData);
                }
               else
               {
//                    ScriptEngine::GetEntityScriptInstance(e1.GetUUID())->InvokeOnCollisionEnter2D(collisionData);

//                    for (const auto& className : sc.ClassName)
//                        ScriptEngine::GetInstance(e1, sc.ClassName)->InvokeOnSensorEnter2D(collisionData);
               }
            }

//            point = worldManifold.points[1];
//            velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
//            velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            collisionData.OtherEntityID = e1.GetUUID();
//            collisionData.RelativeVelocity = { velocityA.x - velocityB.x, velocityA.y - velocityB.y };
            if (e2.has<ScriptComponent>())
            {
                const auto& sc = e2.get<ScriptComponent>();
                if (!aSensor)
                {
                    ScriptEngine::GetEntityScriptInstance(e2.GetUUID())->InvokeOnCollisionEnter2D(collisionData);
//                    for (const auto& className : sc.Classes)
//                        ScriptEngine::GetInstance(e1, className)->InvokeOnCollisionEnter2D(collisionData);
                }
//                else
//                {
////                    ScriptEngine::GetEntityScriptInstance(e2.GetUUID())->InvokeOnCollisionEnter2D(collisionData);
//
////                    for (const auto& className : sc.ClassName)
////                        ScriptEngine::GetInstance(e1, sc.ClassName)->InvokeOnSensorEnter2D(collisionData);
//                }
            }
        }

        void EndContact(b2Contact* contact) final override
        {
            b2Fixture* a = contact->GetFixtureA();
            b2Fixture* b = contact->GetFixtureB();
            bool aSensor = a->IsSensor();
            bool bSensor = b->IsSensor();
            Entity e1 = { (entt::entity)(uint32_t)a->GetBody()->GetUserData().pointer, m_Scene };
            Entity e2 = { (entt::entity)(uint32_t)b->GetBody()->GetUserData().pointer, m_Scene };

//            if (e1.HasComponent<BuoyancyEffector2DComponent>() && aSensor
//                && !e2.HasComponent<BuoyancyEffector2DComponent>() && b->GetBody()->GetType() == b2_dynamicBody)
//            {
//                m_BuoyancyFixtures.erase(eastl::make_pair(a, b));
//            }
//            else if (e2.HasComponent<BuoyancyEffector2DComponent>() && bSensor
//                     && !e1.HasComponent<BuoyancyEffector2DComponent>() && a->GetBody()->GetType() == b2_dynamicBody)
//            {
//                m_BuoyancyFixtures.erase(eastl::make_pair(b, a));
//            }

            b2WorldManifold worldManifold;
            contact->GetWorldManifold(&worldManifold);
            b2Vec2 point = worldManifold.points[0];
            b2Vec2 velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
            b2Vec2 velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            Collision2DData collisionData;
            collisionData.OtherEntityID = e2.GetUUID();
//            collisionData.RelativeVelocity = { velocityB.x - velocityA.x, velocityB.y - velocityA.y };

            if (e1.has<ScriptComponent>())
            {
                const auto& sc = e1.get<ScriptComponent>();
                if (bSensor)
                {
                    ScriptEngine::GetEntityScriptInstance(e1.GetUUID())->InvokeOnCollisionExit2D(collisionData);
//                    for (const auto& className : sc.Classes)
//                        ScriptEngine::GetInstance(e1, className)->InvokeOnCollisionEnter2D(collisionData);
                }
//                else
//                {
////                    ScriptEngine::GetEntityScriptInstance(e1.GetUUID())->InvokeOnCollisionExit2D(collisionData);
//
////                    for (const auto& className : sc.ClassName)
////                        ScriptEngine::GetInstance(e1, sc.ClassName)->InvokeOnSensorExit2D(collisionData);
//                }
            }

            point = worldManifold.points[1];
            velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
            velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            collisionData.OtherEntityID = e1.GetUUID();
//            collisionData.RelativeVelocity = { velocityA.x - velocityB.x, velocityA.y - velocityB.y };
            if (e2.has<ScriptComponent>())
            {
                const auto& sc = e2.get<ScriptComponent>();
                if (aSensor)
                {
                    ScriptEngine::GetEntityScriptInstance(e2.GetUUID())->InvokeOnCollisionExit2D(collisionData);
//                    for (const auto& className : sc.Classes)
//                        ScriptEngine::GetInstance(e1, className)->InvokeOnCollisionEnter2D(collisionData);
                }
//                else
//                {
////                    ScriptEngine::GetEntityScriptInstance(e1.GetUUID())->InvokeOnCollisionExit2D(collisionData);
//
////                    for (const auto& className : sc.ClassName)
////                        ScriptEngine::GetInstance(e1, sc.ClassName)->InvokeOnSensorExit2D(collisionData);
//                }
            }
        }
    };

    Scene::Scene(bool initalize) : m_oApp(Application::Get())
    {
        if (!initalize)
            return;
//        m_Registry.on_construct<ScriptComponent>().connect<&Scene::OnScriptComponentConstruct>(this);
//        m_Registry.on_destroy<ScriptComponent>().connect<&Scene::OnScriptComponentDestroy>(this);

        // This might not be the the best way, but Audio Engine needs to keep track of all audio component
        // to have an easy way to lookup a component associated with active sound.
//        m_Registry.on_construct<AudioComponent>().connect<&Scene::OnAudioComponentConstruct>(this);
//        m_Registry.on_destroy<AudioComponent>().connect<&Scene::OnAudioComponentDestroy>(this);

        m_Registry.on_construct<CameraComponent>().connect<&Scene::OnCameraComponentConstruct>(this);

        m_Registry.on_construct<Rigidbody2D>().connect<&Scene::OnRigidBody2DComponentConstruct>(this);
        m_Registry.on_destroy<Rigidbody2D>().connect<&Scene::OnRigidBody2DComponentDestroy>(this);
        m_Registry.on_construct<BoxCollider2D>().connect<&Scene::OnBoxCollider2DComponentConstruct>(this);
        m_Registry.on_destroy<BoxCollider2D>().connect<&Scene::OnBoxCollider2DComponentDestroy>(this);
        m_Registry.on_construct<CircleCollider2D>().connect<&Scene::OnCircleCollider2DComponentConstruct>(this);
        m_Registry.on_destroy<CircleCollider2D>().connect<&Scene::OnCircleCollider2DComponentDestroy>(this);
    }

    Scene::~Scene()
    {
        // Clear the registry so that all callbacks are called
        m_Registry.clear();

        // Disconnect EnTT callbacks
        m_Registry.on_construct<ScriptComponent>().disconnect(this);
        m_Registry.on_destroy<ScriptComponent>().disconnect(this);

//        m_Registry.on_construct<AudioComponent>().disconnect();
//        m_Registry.on_destroy<AudioComponent>().disconnect();

        m_Registry.on_construct<Rigidbody2D>().disconnect();
        m_Registry.on_destroy<Rigidbody2D>().disconnect();

        delete m_PhysicsWorld;
    }

//    void Scene::init_systems()
//    {

//        get_world().system<fox::Button, Animator>().each([](Entity e, fox::Button& btn, Animator& anim)
//        {
//            if (btn.state() == btn.prev_state())
//                return;
//            switch (btn.state())
//            {
//                case fox::BtnState::NORMAL:
//                    anim.switch_to("Normal");
//                    break;
//                case fox::BtnState::HOVER:
//                    anim.switch_to("Hover");
//                    break;
//                case fox::BtnState::PRESSED:
//                    anim.switch_to("Pressed");
//                    break;
//                case fox::BtnState::DISABLE:
//                    anim.switch_to("Disable");
//                    break;
//                default:
//                    anim.switch_to("Normal");
//                    break;
//            }
//        });

//        get_world().system<fox::Button, fox::Transform>().kind(ecs::OnStore)
//            .each([](Entity e, fox::Button& btn, fox::Transform& transform)
//        {
//            ray::Rectangle sourceRec = { 0.0f, 0.0f, (float)btn.sprite().width, (float)btn.sprite().height };
//            ray::Rectangle destRec = { transform.position.x, transform.position.y, btn.sprite().width * transform.scale.x, btn.sprite().height * transform.scale.y };
//            bool btnClick = false;
//            if (CheckCollisionPointRec(fox::Input::mouse_pos(), destRec))
//            {
//                btn.set_state(fox::BtnState::HOVER);
//                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
//                    btnClick = true;
//            } else {
//                btn.set_state(fox::BtnState::NORMAL);
//            }
//
//            if (btnClick)
//            {
//                btn.click();
//                btn.set_state(fox::BtnState::PRESSED);
//            }
//            btn.sprite().Draw(sourceRec, destRec, Vec2(), transform.rotation.GetAngle());
//        });


    Entity Scene::NewEntity(const std::string &name)
    {
        return NewEntityWithUUID(UUID(), name);
    }

    Entity Scene::NewChildEntity(Entity parent, const std::string& name)
    {
//        FOX_PROFILE_FUNC();

        Entity entity = NewEntityWithUUID(UUID(), name);
        if (parent)
            entity.SetParent(parent);
        return entity;
    }

    Entity Scene::NewEntityWithUUID(UUID uuid, const std::string &name)
    {
        //        FOX_PROFILE_FUNC();
        Entity entity = { m_Registry.create(), this };
        auto& tag = entity.add<NameComponent>();
        tag.name = name.empty() ? "Entity" : name;

        entity.add<IDComponent>(uuid);
        entity.add<TransformComponent>();
        entity.add<HierarchyComponent>();

        FOX_CORE_ASSERT(m_EntityMap.find(uuid) == m_EntityMap.end());
        m_EntityMap[uuid] = entity;
        SortEntities();
        return entity;
    }

    Entity Scene::GetEntityByUUID(UUID uuid)
    {
        if (uuid == UUID::Empty())
            return {};
//        FOX_PROFILE_FUNC();
        FOX_CORE_ASSERT(m_EntityMap.find(id) != m_EntityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");
        return { m_EntityMap.at(uuid), this };
    }

    Entity Scene::TryGetEntityByUUID(UUID uuid)
    {
//        FOX_PROFILE_FUNC();
        try {
            return GetEntityByUUID(uuid);
        } catch (const std::out_of_range& e) {
            return Entity {};
        }
    }

    Entity Scene::FindEntityByName(std::string_view name)
    {
        auto view = m_Registry.view<NameComponent>();
        for (auto entity : view)
        {
            const NameComponent& tc = view.get<NameComponent>(entity);
            if (tc.name == name)
                return Entity{ entity, this };
        }
        return {};
    }

    void Scene::SubmitToDestroyEntity(Entity entity)
    {
        SubmitPostUpdateFunc([entity]() { entity.m_Scene->DestroyEntity(entity); });
    }

    void Scene::DestroyEntity(Entity entity, bool excludeChildren, bool first)
    {
        bool isValid = m_Registry.valid((entt::entity)entity);
        if (!isValid)
        {
            FOX_CORE_WARN_TAG("Scene", "Trying to destroy invalid entity! entt=%", (uint32_t)entity);
            return;
        }

//        if (entity.has<ScriptComponent>())
//            ScriptEngine::ShutdownScriptEntity(entity);

//        if (entity.has<AudioComponent>())
//            MiniAudioEngine::Get().UnregisterAudioComponent(m_SceneID, entity.GetUUID());

        if (m_IsRunning)
        {
            if (entity.has<Rigidbody2D>())
            {
                b2Body* body = (b2Body*)entity.get<Rigidbody2D>().RuntimeBody;
                m_PhysicsWorld->DestroyBody(body);
            }
        }

//        if (m_OnEntityDestroyedCallback)
//            m_OnEntityDestroyedCallback(entity);

        if (!excludeChildren)
        {
            for (size_t i = 0; i < entity.Children().size(); i++)
            {
                auto childId = entity.Children()[i];
                Entity child = GetEntityByUUID(childId);
                DestroyEntity(child, excludeChildren, false);
            }
        }

        if (first)
        {
            if (auto parent = entity.GetParent(); parent)
                parent.RemoveChild(entity);
        }

        UUID id = entity.GetUUID();
        m_EntityMap.erase(id);
        m_Registry.destroy(entity);
        SortEntities();
    }


    void Scene::ParentEntity(Entity entity, Entity parent)
    {
//        FOX_PROFILE_FUNC();

        if (parent.IsDescendantOf(entity))
        {
            UnparentEntity(parent);

            Entity newParent = TryGetEntityByUUID(entity.GetParentUUID());
            if (newParent)
            {
                UnparentEntity(entity);
                ParentEntity(parent, newParent);
            }
        }
        else
        {
            Entity previousParent = TryGetEntityByUUID(entity.GetParentUUID());

            if (previousParent)
                UnparentEntity(entity);
        }

        entity.SetParentUUID(parent.GetUUID());
        parent.Children().push_back(entity.GetUUID());

        ConvertToLocalSpace(entity);
    }

    void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
    {
//        FOX_PROFILE_FUNC();

        Entity parent = TryGetEntityByUUID(entity.GetParentUUID());
        if (!parent)
            return;

        auto& parentChildren = parent.Children();
        parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()), parentChildren.end());

        if (convertToWorldSpace)
            ConvertToWorldSpace(entity);

        entity.SetParentUUID(0);
    }

    const std::unordered_map<UUID, entt::entity>& Scene::GetEntities() const
    {
        return m_EntityMap;
    }

    void Scene::ConvertToLocalSpace(Entity entity)
    {
//        FOX_PROFILE_FUNC();

        Entity parent = TryGetEntityByUUID(entity.GetParentUUID());
        if (!parent)
            return;

        auto& transform = entity.GetTransform();
        glm::mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);
        glm::mat4 localTransform = glm::inverse(parentTransform) * transform.GetTransform();
        transform.SetTransform(localTransform);
    }

    void Scene::ConvertToWorldSpace(Entity entity)
    {
//        FOX_PROFILE_FUNC();

        Entity parent = TryGetEntityByUUID(entity.GetParentUUID());
        if (!parent)
            return;

        glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
        auto& entityTransform = entity.GetTransform();
        entityTransform.SetTransform(transform);
    }

    glm::mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
    {
//        FOX_PROFILE_FUNC();

        glm::mat4 transform(1.0f);

        Entity parent = TryGetEntityByUUID(entity.GetParentUUID());
        if (parent)
            transform = GetWorldSpaceTransformMatrix(parent);

        return transform * entity.GetTransform().GetTransform();
    }

    // TODO: Definitely cache this at some point
    TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
    {
//        FOX_PROFILE_FUNC();

        glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
        TransformComponent transformComponent;
        transformComponent.SetTransform(transform);
        return transformComponent;
    }

    void Scene::OnUpdateEditor(EditorCamera &camera, Timestep ts)
    {
        // Update Animation
        {
            auto view = m_Registry.view<SpriteRenderer, Animator>();
            for (auto entity : view)
            {
                auto [spriteRenderer, animator] = view.get<SpriteRenderer, Animator>(entity);
                animator.Update(ts);

                if (animator.CurrentAnimation && animator.CurrentAnimation->CurrentFrame)
                {
                    AssetHandle textureID = animator.CurrentAnimation->CurrentFrame->Texture;
                    if (AssetManager::IsAssetHandleValid(textureID))
                    {
                        spriteRenderer.Sprite = textureID;
                    }
                }
            }
        }

        RenderScene(camera);
    }

    void Scene::OnUpdateSimulation(EditorCamera& camera, Timestep ts)
    {
        // Physics
        {
            // Copies transform from Fox to Box2D
			{
                auto view = m_Registry.view<Rigidbody2D>();
                for (auto e : view)
                {
                    Entity entity = { e, this };
                    auto& transform = entity.get<TransformComponent>();
                    auto& rb2d = entity.get<Rigidbody2D>();

                    b2Body* body = (b2Body*)rb2d.RuntimeBody;
                    glm::vec3 position = transform.position;
                    float angle = transform.GetRotation().z;

                    const auto& bodyPosition = body->GetPosition();
                    const float bodyAngle = body->GetAngle();

                    bool awake = bodyPosition.x != transform.position.x || bodyPosition.y != transform.position.y || bodyAngle != angle;

                    body->SetTransform({ position.x, position.y }, angle);

                    if (awake)
                        body->SetAwake(true);
                }
            }

            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2D>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                auto& transform = entity.get<TransformComponent>();
                auto& rb2d = entity.get<Rigidbody2D>();

                b2Body* body = (b2Body*)rb2d.RuntimeBody;
                const auto& position = body->GetPosition();
                transform.position.x = position.x;
                transform.position.y = position.y;

                auto rotation = transform.GetRotation();
                rotation.z = body->GetAngle();
                transform.SetRotation(rotation);
            }
        }

        // Render
        RenderScene(camera);
    }

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        // Update scripts
        {
            // C# Entity OnUpdate
            auto view = m_Registry.view<ScriptComponent>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                ScriptEngine::OnUpdateEntity(entity, ts);
            }

//            m_Registry.view<NativeScript>().each([=](auto entity, auto& nsc)
//              {
//                  // TODO: Move to Scene::OnScenePlay
//                  if (!nsc.Instance)
//                  {
//                      nsc.Instance = nsc.InstantiateScript();
//                      nsc.Instance->m_Entity = Entity{ entity, this };
//                      nsc.Instance->OnCreate();
//                  }
//
//                  nsc.Instance->OnUpdate(ts);
//              });
        }

        // Update ParticleSystem
        {
            auto view = m_Registry.view<TransformComponent, ParticleSystem>();
            for (auto entity : view) {
                auto [transform, particleSystem] = view.get<TransformComponent, ParticleSystem>(entity);

                if (!particleSystem.Play)
                    continue;

                particleSystem.ParticleSettings.Position = transform.position;

                // Update the time that has passed between intervals
                particleSystem.accumulator += ts;
                while (particleSystem.accumulator >= 1.0f / particleSystem.ParticleSettings.RateOverTime) {
                    particleSystem.Emit();
                    particleSystem.accumulator -= 1.0f / particleSystem.ParticleSettings.RateOverTime;
                }

//                if (particleSystem.intervalTime >= particleSystem.releaseInterval) {
//                    particleSystem.intervalTime = 0;
//                    for (int i = 0; i < 3; i++)
//                        particleSystem.Emit();
//                }
                particleSystem.Update(ts);
            }
        }

        // Update Animation
        {
            auto view = m_Registry.view<SpriteRenderer, Animator>();
            for (auto entity : view)
            {
                auto [spriteRenderer, animator] = view.get<SpriteRenderer, Animator>(entity);
                animator.Update(ts);

                if (animator.CurrentAnimation && animator.CurrentAnimation->CurrentFrame)
                {
                    AssetHandle textureID = animator.CurrentAnimation->CurrentFrame->Texture;
                    if (AssetManager::IsAssetHandleValid(textureID))
                    {
                        spriteRenderer.Sprite = textureID;
                    }
                }
            }
        }

        // Physics
        {
            // Copies transform from Fox to Box2D
			{
                auto view = m_Registry.view<Rigidbody2D>();
                for (auto e : view)
                {
                    Entity entity = { e, this };
                    auto& transform = entity.get<TransformComponent>();
                    auto& rb2d = entity.get<Rigidbody2D>();

                    b2Body* body = (b2Body*)rb2d.RuntimeBody;
                    glm::vec3 position = transform.position;
                    float angle = transform.GetRotation().z;

                    const auto& bodyPosition = body->GetPosition();
                    const float bodyAngle = body->GetAngle();

                    bool awake = bodyPosition.x != transform.position.x || bodyPosition.y != transform.position.y || bodyAngle != angle;

                    body->SetTransform({ position.x, position.y }, angle);

                    if (awake)
                        body->SetAwake(true);
                }
            }

            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2D>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                auto& transform = entity.get<TransformComponent>();
                auto& rb2d = entity.get<Rigidbody2D>();

                b2Body* body = (b2Body*)rb2d.RuntimeBody;
                const auto& position = body->GetPosition();
                transform.position.x = position.x;
                transform.position.y = position.y;
                auto rotation = transform.GetRotation();
                rotation.z = body->GetAngle();
                transform.SetRotation(rotation);
            }
        }

        // Render 2D
        Entity cameraEntity = GetPrimaryCameraEntity();
        if (cameraEntity)
        {
            Camera& mainCamera = cameraEntity.get<CameraComponent>().camera;
            glm::mat4 cameraTransform = cameraEntity.get<TransformComponent>().GetTransform();

            Renderer2D::BeginScene(mainCamera.GetProjection() * glm::inverse(cameraTransform));
            RenderScene();
            Renderer2D::EndScene();
        }

        ProcessPostQueue();
    }

    void Scene::OnRuntimeStart()
    {
        m_IsRunning = true;

        OnPhysics2DStart();

        // Scripting
        {
            // Instantiate all script entities
            auto view = m_Registry.view<ScriptComponent>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                ScriptEngine::OnCreateEntity(entity);
            }
        }
    }

    void Scene::ProcessPostQueue()
    {
        for (auto&& fn : m_PostUpdateQueue)
            fn();
        m_PostUpdateQueue.clear();
    }

    void Scene::OnRuntimeStop()
    {
        m_IsRunning = false;

        OnPhysics2DStop();
        ScriptEngine::OnRuntimeStop();
    }

    void Scene::OnSimulationStart()
    {
        OnPhysics2DStart();
    }

    void Scene::OnSimulationStop()
    {
        OnPhysics2DStop();
    }

    static PhysicsContactListener2D m_ContactListener;

    void Scene::OnPhysics2DStart()
    {
        m_ContactListener.m_Scene = this;

        m_PhysicsWorld = new b2World({ 0.0f, -9.81f });
        m_PhysicsWorld->SetContactListener(&m_ContactListener);

        auto view = m_Registry.view<Rigidbody2D>();
        for (auto e : view)
        {
            Entity entity = { e, this };
            UUID entityID = entity.GetUUID();
            auto& transform = entity.get<TransformComponent>();
            auto& rb2d = entity.get<Rigidbody2D>();

            b2BodyDef bodyDef;
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
            bodyDef.position.Set(transform.position.x, transform.position.y);
            bodyDef.angle = transform.GetRotation().z;

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.FixedRotation);

            b2MassData massData;
            body->GetMassData(&massData);
            massData.mass = rb2d.Mass;
            body->SetMassData(&massData);
            body->SetGravityScale(rb2d.GravityScale);
            body->SetLinearDamping(rb2d.LinearDrag);
            body->SetAngularDamping(rb2d.AngularDrag);
            body->SetBullet(rb2d.IsBullet);
            body->GetUserData().pointer = (uint32_t)entity;
            rb2d.RuntimeBody = body;

            PhysicLayer* physicsLayer = nullptr;
            if (!Physics2D::GetLayer(rb2d.LayerID, physicsLayer))
            {
                FOX_WARN("Invalid Layer ID (%). Set the layer to default.", rb2d.LayerID);
                Physics2D::GetLayer(Physics2D::DefaultLayer, physicsLayer);
            }

            if (entity.has<BoxCollider2D>())
            {
                auto& bc2d = entity.get<BoxCollider2D>();

                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.Size.x * transform.scale.x, bc2d.Size.y * transform.scale.y);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.Density;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                fixtureDef.filter.categoryBits = physicsLayer->AsBit();
                fixtureDef.filter.maskBits = physicsLayer->Collisions;
                body->CreateFixture(&fixtureDef);
            }

            if (entity.has<CircleCollider2D>())
            {
                auto& cc2d = entity.get<CircleCollider2D>();

                b2CircleShape circleShape;
                circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
                circleShape.m_radius = transform.scale.x * cc2d.Radius;

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &circleShape;
                fixtureDef.density = cc2d.Density;
                fixtureDef.friction = cc2d.Friction;
                fixtureDef.restitution = cc2d.Restitution;
                fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    void Scene::OnPhysics2DStop()
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }

    void Scene::RenderScene(EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);
        RenderScene();
        Renderer2D::EndScene();
    }

    void Scene::RenderScene()
    {
        // Draw sprites
        {
            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderer>);
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRenderer>(entity);

                Entity e = Entity(entity, this);
                glm::mat4 transformMatrice = GetWorldSpaceTransformMatrix(e);
                Renderer2D::DrawSprite(transformMatrice, sprite, (int)entity);
            }
        }

        // Draw circles
        {
            auto view = m_Registry.view<TransformComponent, CircleRenderer>();
            for (auto entity : view)
            {
                auto [transform, circle] = view.get<TransformComponent, CircleRenderer>(entity);

                Entity e = Entity(entity, this);
                glm::mat4 transformMatrice = GetWorldSpaceTransformMatrix(e);
                Renderer2D::DrawCircle(transformMatrice, circle.Color, circle.Thickness, circle.Fade, (int)entity);
            }
        }

        // Draw Particle System
        {
            auto view = m_Registry.view<TransformComponent, ParticleSystem>();
            for (auto entity : view)
            {
                auto [transform, particleSystem] = view.get<TransformComponent, ParticleSystem>(entity);
                Renderer2D::DrawParticle(transform.GetTransform(), particleSystem);
            }
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        if (m_ViewportWidth == width && m_ViewportHeight == height)
            return;

        m_ViewportHeight = height;
        m_ViewportWidth = width;

        // Resize our non-FixedAspectRatio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.camera.SetViewportSize(width, height);
        }
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            const auto& camera = view.get<CameraComponent>(entity);
            if (camera.Primary)
                return Entity{entity, this};
        }
        return {};
    }

    void Scene::OnCameraComponentConstruct(entt::registry& registry, entt::entity entity)
    {
        Entity e = { entity, this };
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            e.get<CameraComponent>().camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    void Scene::OnRigidBody2DComponentConstruct(entt::registry& registry, entt::entity entity)
    {
//        FOX_PROFILE_FUNC();

        if (!m_IsRunning)
            return;

        Entity e = { entity, this };
        UUID entityID = e.get<IDComponent>().ID;
        TransformComponent& transform = e.get<TransformComponent>();
        auto& rb2d = registry.get<Rigidbody2D>(entity);

        b2BodyDef bodyDef;

        bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef.position.Set(transform.position.x, transform.position.y);
        bodyDef.angle = transform.GetRotation().z;

        b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
        body->SetFixedRotation(rb2d.FixedRotation);

        b2MassData massData;
        body->GetMassData(&massData);
        massData.mass = rb2d.Mass;
        body->SetMassData(&massData);
        body->SetGravityScale(rb2d.GravityScale);
        body->SetLinearDamping(rb2d.LinearDrag);
        body->SetAngularDamping(rb2d.AngularDrag);
        body->SetBullet(rb2d.IsBullet);
        body->GetUserData().pointer = (uint32_t)entity;

        rb2d.RuntimeBody = body;
    }

    void Scene::OnRigidBody2DComponentDestroy(entt::registry& registry, entt::entity entity)
    {
//        FOX_PROFILE_FUNC();

        if (!m_IsRunning)
            return;
    }

    void Scene::OnBoxCollider2DComponentConstruct(entt::registry& registry, entt::entity entity)
    {
//        FOX_PROFILE_FUNC();

        if (!m_IsRunning)
            return;

        Entity e = { entity, this };
        auto& transform = e.GetTransform();

        auto& bc2d = registry.get<BoxCollider2D>(entity);
        if (e.has<Rigidbody2D>())
        {
            auto& rigidBody2D = e.get<Rigidbody2D>();
            FOX_CORE_ASSERT(rigidBody2D.RuntimeBody);
            b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

            b2PolygonShape boxShape;
            boxShape.SetAsBox(bc2d.Size.x * transform.scale.x, bc2d.Size.y * transform.scale.y);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &boxShape;
            fixtureDef.density = bc2d.Density;
            fixtureDef.friction = bc2d.Friction;
            fixtureDef.restitution = bc2d.Restitution;
            fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
            body->CreateFixture(&fixtureDef);
        }
    }

    void Scene::OnBoxCollider2DComponentDestroy(entt::registry& registry, entt::entity entity)
    {
    }

    void Scene::OnCircleCollider2DComponentConstruct(entt::registry& registry, entt::entity entity)
    {
//        FOX_PROFILE_FUNC();

        if (!m_IsRunning)
            return;

        Entity e = { entity, this };
        auto& transform = e.GetTransform();

        auto& cc2d = registry.get<CircleCollider2D>(entity);
        if (e.has<Rigidbody2D>())
        {
            auto& rigidBody2D = e.get<Rigidbody2D>();
            FOX_CORE_ASSERT(rigidBody2D.RuntimeBody);
            b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

            b2CircleShape circleShape;
            circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
            circleShape.m_radius = transform.scale.x * cc2d.Radius;

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &circleShape;
            fixtureDef.density = cc2d.Density;
            fixtureDef.friction = cc2d.Friction;
            fixtureDef.restitution = cc2d.Restitution;
            fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
            body->CreateFixture(&fixtureDef);
        }
    }

    void Scene::OnCircleCollider2DComponentDestroy(entt::registry& registry, entt::entity entity)
    {
    }

    template<typename... Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        ([&]()
        {
            auto view = src.view<Component>();
            for (auto srcEntity : view)
            {
                entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

                auto& srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);
            }
        }(), ...);
    }

    template<typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template<typename... Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        ([&]()
        {
            if (src.has<Component>())
                dst.template add_or_replace<Component>(src.template get<Component>());
        }(), ...);
    }

    template<typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        CopyComponentIfExists<Component...>(dst, src);
    }

    void Scene::CopyAllComponentsIfExists(Entity dst, Entity src)
    {
        CopyComponentIfExists(AllComponents{}, dst, src);
    }


    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = new_ref<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;

        // Create entities in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<NameComponent>(e).name;
            Entity newEntity = newScene->NewEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        // Copy components (except IDComponent and TagComponent)
        CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }

    void Scene::DuplicateEntity(Entity entity)
    {
        Entity newEntity = NewEntity(entity.GetName());
        CopyComponentIfExists(AllComponents{}, newEntity, entity);
    }

    Entity Scene::CloneEntity(Entity entity, Entity parent, const glm::vec3* translation, const glm::vec3* rotation, const glm::vec3* scale)
    {
//        FOX_PROFILE_FUNC();
//        FOX_CORE_VERIFY(entity.HasComponent<PrefabComponent>());
        std::string name = entity.GetName();
        Entity newEntity = NewEntity(name);
        if (parent)
            newEntity.SetParent(parent);
        CopyComponentIfExists(AllComponents{}, newEntity, entity);

        if (translation)
            newEntity.GetTransform().position = *translation;
        if (rotation)
            newEntity.GetTransform().SetRotation(*rotation);
        if (scale)
            newEntity.GetTransform().scale = *scale;

        // Create children
        for (auto childId : entity.Children())
            CloneEntity(entity.m_Scene->GetEntityByUUID(childId), newEntity);

        if (newEntity.has<ScriptComponent>() && m_IsRunning)
            ScriptEngine::OnEntityInstantiated(newEntity, entity);
        return newEntity;
    }

    Entity Scene::Instantiate(Ref<Prefab> prefab, const glm::vec3* translation, const glm::vec3* rotation, const glm::vec3* scale)
    {
//        FOX_PROFILE_FUNC();

        Entity result;

        // TODO: we need a better way of retrieving the "root" entity
        auto entities = prefab->m_Scene->GetAllEntitiesWith<HierarchyComponent>();
        for (auto e : entities)
        {
            Entity entity = { e, prefab->m_Scene.Raw() };
            if (!entity.GetParent())
            {
                result = CloneEntity(entity, Entity(), translation, rotation, scale);
                break;
            }
        }
        return result;
    }

    void Scene::SortEntities()
    {
        m_Registry.sort<IDComponent>([&](const auto lhs, const auto rhs)
                                     {
                                         auto lhsEntity = m_EntityMap.find(lhs.ID);
                                         auto rhsEntity = m_EntityMap.find(rhs.ID);
                                         return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
                                     });
    }

    Ref<Scene> Scene::CreateEmpty()
    {
        return new_ref<Scene>(false);
    }


}// namespace fox
