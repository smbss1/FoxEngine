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
#include "Components/EntityName.hpp"
#include "Components.hpp"
#include "Ecs/Entity.hpp"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_contact.h"
#include "Core/Assert.hpp"
#include "Scripting/ScriptEngine.hpp"

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
//            BoxCollider2D* bodyUserData_1 = reinterpret_cast<BoxCollider2D*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
//            BoxCollider2D* bodyUserData_2 = reinterpret_cast<BoxCollider2D*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
//
//            // check if fixture A was a ball
//            if (bodyUserData_1 && bodyUserData_2)
//            {
//                bodyUserData_1->OnTriggerEnter(*bodyUserData_2);
//                bodyUserData_2->OnTriggerEnter(*bodyUserData_1);
////                uint64_t id_1 = (uint64_t)*bodyUserData_1;
////                uint64_t id_2 = (uint64_t)*bodyUserData_2;
////
////                FOX_CORE_INFOINFOINFOINFOINFOINFOINFOINFOINFOINFOINFO("Id 1: %", id_1);
////                FOX_CORE_INFO("Id 2: %", id_2);
////
////                Entity entity_1 = m_Scene->GetEntityByUUID(id_1);
////                Entity entity_2 = m_Scene->GetEntityByUUID(id_2);
////
////                FOX_CORE_INFO("Entity Id 1: %", entity_1.GetUUID());
////                FOX_CORE_INFO("Entity Id 2: %", entity_2.GetUUID());
////
////                entity_1.get<BoxCollider2D>().OnTriggerEnter(entity_2.get<BoxCollider2D>());
////                entity_2.get<BoxCollider2D>().OnTriggerEnter(entity_1.get<BoxCollider2D>());
//            }

            b2Fixture* a = contact->GetFixtureA();
            b2Fixture* b = contact->GetFixtureB();

            bool aSensor = a->IsSensor();
            bool bSensor = b->IsSensor();

            Entity e1 = { (entt::entity)(uint32_t)a->GetUserData().pointer, m_Scene };
            Entity e2 = { (entt::entity)(uint32_t)b->GetUserData().pointer, m_Scene };

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
//                else
//                {
////                    ScriptEngine::GetEntityScriptInstance(e1.GetUUID())->InvokeOnCollisionEnter2D(collisionData);
//
////                    for (const auto& className : sc.ClassName)
////                        ScriptEngine::GetInstance(e1, sc.ClassName)->InvokeOnSensorEnter2D(collisionData);
//                }
            }

//            point = worldManifold.points[1];
//            velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
//            velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            collisionData.OtherEntityID = e1.GetUUID();
//            collisionData.RelativeVelocity = { velocityA.x - velocityB.x, velocityA.y - velocityB.y };
            if (e2.has<ScriptComponent>())
            {
                const auto& sc = e2.get<ScriptComponent>();
                if (aSensor)
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
            void* bodyUserData_1 = (void *) contact->GetFixtureA()->GetBody()->GetUserData().pointer;
            void* bodyUserData_2 = (void *) contact->GetFixtureB()->GetBody()->GetUserData().pointer;

//            // check if fixture A was a ball
//            if (bodyUserData_1)
//            {
//                static_cast<Rigidbody2D*>(bodyUserData_1)->endContact();
//            }
//
//            // check if fixture B was a ball
//            if (bodyUserData_2)
//            {
//                static_cast<Rigidbody2D*>(bodyUserData_2)->endContact();
//            }

            b2Fixture* a = contact->GetFixtureA();
            b2Fixture* b = contact->GetFixtureB();
            bool aSensor = a->IsSensor();
            bool bSensor = b->IsSensor();
            Entity e1 = { (entt::entity)(uint32_t)a->GetUserData().pointer, m_Scene };
            Entity e2 = { (entt::entity)(uint32_t)b->GetUserData().pointer, m_Scene };

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

    Scene::Scene() : m_oApp(Application::Get())
    {
    }

    Scene::~Scene()
    {
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

    Entity Scene::NewEntityWithUUID(UUID uuid, const std::string &name)
    {
        Entity entity = { m_Registry.create(), this };
        entity.add<IDComponent>(uuid);
        entity.add<TransformComponent>();
        auto& tag = entity.add<EntityName>();
        tag.name = name.empty() ? "Entity" : name;

        m_EntityMap[uuid] = entity;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_EntityMap.erase(entity.GetUUID());
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdateEditor(EditorCamera &camera)
    {
        RenderScene(camera);
    }

    void Scene::OnUpdateSimulation(EditorCamera& camera, Timestep ts)
    {
        // Physics
        {
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
                transform.rotation.z = body->GetAngle();
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


        // Physics
        {
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
                transform.rotation.z = body->GetAngle();
            }
        }

        // Render 2D
        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

                if (camera.Primary)
                {
                    mainCamera = &camera.camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }

        if (mainCamera)
        {
            Renderer2D::BeginScene(*mainCamera, cameraTransform);
            RenderScene();
            Renderer2D::EndScene();
        }
    }

    void Scene::OnRuntimeStart()
    {
        m_IsRunning = true;

        OnPhysics2DStart();

        // Scripting
        {
            ScriptEngine::OnRuntimeStart(this);

            // Instantiate all script entities
            auto view = m_Registry.view<ScriptComponent>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                ScriptEngine::OnCreateEntity(entity);
            }
        }
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
            auto& transform = entity.get<TransformComponent>();
            auto& rb2d = entity.get<Rigidbody2D>();

            b2BodyDef bodyDef;
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
            bodyDef.position.Set(transform.position.x, transform.position.y);
            bodyDef.angle = transform.rotation.z;

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;
            UUID id = entity.GetUUID();

            if (entity.has<BoxCollider2D>())
            {
                auto& bc2d = entity.get<BoxCollider2D>();
                body->GetUserData().pointer = reinterpret_cast<uintptr_t>(&bc2d);

                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.Size.x * transform.scale.x, bc2d.Size.y * transform.scale.y);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.Density;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                fixtureDef.userData.pointer = (uint32_t)entity;
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
                fixtureDef.userData.pointer = (uint32_t)entity;
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

                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }
        }

        // Draw circles
        {
            auto view = m_Registry.view<TransformComponent, CircleRenderer>();
            for (auto entity : view)
            {
                auto [transform, circle] = view.get<TransformComponent, CircleRenderer>(entity);

                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
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

    Application &Scene::GetApp()
    {
        return m_oApp;
    }

    Entity Scene::GetEntityByUUID(UUID uuid)
    {
        // TODO(Yan): Maybe should be assert
        if (m_EntityMap.find(uuid) != m_EntityMap.end())
            return { m_EntityMap.at(uuid), this };

        return {};
    }

    Entity Scene::FindEntityByName(std::string_view name)
    {
        auto view = m_Registry.view<EntityName>();
        for (auto entity : view)
        {
            const EntityName& tc = view.get<EntityName>(entity);
            if (tc.name == name)
                return Entity{ entity, this };
        }
        return {};
    }

    template<typename T>
    void Scene::OnComponentAdded(Entity &e, T &component) { }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity &e, TransformComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<SpriteRenderer>(Entity &e, SpriteRenderer& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<NativeScript>(Entity &e, NativeScript& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<EntityName>(Entity &e, EntityName& component)
    {
    }

//    template<>
//    void Scene::OnComponentAdded<EntityTag>(Entity &e, EntityTag& component)
//    {
//    }

//    template<>
//    void Scene::OnComponentAdded<AnimationPlayer>(Entity &e, AnimationPlayer& component)
//    {
//    }

    template<>
    void Scene::OnComponentAdded<Rigidbody2D>(Entity &e, Rigidbody2D& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<BoxCollider2D>(Entity &e, BoxCollider2D& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CircleCollider2D>(Entity &e, CircleCollider2D& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CircleRenderer>(Entity &e, CircleRenderer& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<ScriptComponent>(Entity &e, ScriptComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity &e, CameraComponent& component)
    {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
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

    ref<Scene> Scene::Copy(ref<Scene> other)
    {
        ref<Scene> newScene = new_ref<Scene>();

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
            const auto& name = srcSceneRegistry.get<EntityName>(e).name;
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

    Entity Scene::CloneEntity(Entity& entity)
    {
        std::string name = entity.GetName();
        Entity newEntity = NewEntity(name);
        CopyComponentIfExists(AllComponents{}, newEntity, entity);

        if (newEntity.has<ScriptComponent>() && m_IsRunning)
            ScriptEngine::OnCreateEntity(newEntity);
        return newEntity;
    }

}// namespace fox
