/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Scene.cpp
*/

#include <Renderer/Camera.hpp>
#include <Components/CameraComponent.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Components/NativeScript.hpp>
#include <Components/EntityName.hpp>
#include "Core/Scene.hpp"
#include "Components.hpp"
#include "Core/Application.hpp"

namespace fox
{
    Scene::Scene(Application& app) : m_oApp(app), m_oWorld()
    {
        m_oWorld.RegisterComponent<TransformComponent>();
        m_oWorld.RegisterComponent<SpriteRenderer>();
        m_oWorld.RegisterComponent<NativeScript>();
        m_oWorld.RegisterComponent<CameraComponent>();
        m_oWorld.RegisterComponent<EntityTag>();
        m_oWorld.RegisterComponent<EntityName>();

        // Draw Systems
        m_oWorld.system<TransformComponent, SpriteRenderer>().kind(ecs::OnStore)
                .each([](Entity& e, TransformComponent& transform, SpriteRenderer& sprite)
                      {
                          Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int) e.get_id());
                      });


        // Native Script Systems
        m_oWorld.system<NativeScript>().kind(ecs::OnUpdate)
                .each([&](Entity& e, NativeScript& script)
                      {
                          script.on_update();
                      });

        m_oWorld.system<NativeScript>().kind(ecs::OnAdd)
                .each([&](Entity& e, NativeScript& script)
                      {
                          script.m_pWorld = e.get_world();
                          script.on_create_all(e.get_id(), app);
                      });

        m_oWorld.system<NativeScript>().kind(ecs::OnAddScript)
                .each([&](Entity& e, NativeScript& script)
                      {
                          script.on_create(e.get_id(), app);
                      });
    }

//    void Scene::init_systems()
//    {
//        get_world().system<NativeScript, BoxCollider>()
//                .kind(ecs::OnAdd)
//                .each([](Entity e, NativeScript& script, BoxCollider& shape)
//                      {
//                          shape.m_pScript = &script;
//                      });
//        get_world().system<BoxCollider>()
//                .kind(ecs::OnAdd)
//                .each([](Entity e, BoxCollider& shape)
//                      {
//                          shape.entity = e.get_id();
//                      });
//
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
//
//        get_world().system<fox::Transform>().kind(fox::ecs::PreUpdate)
//            .each([](Entity e, fox::Transform& transform)
//        {
//            transform.update_position();
//        });
//
//        get_world().system<ray::Model, fox::Transform>().kind(fox::game::CameraDraw)
//            .each([](Entity e, ray::Model& model, fox::Transform& transform)
//        {
//            model.Draw(transform.position, transform.rotation.normalized().getAxis(), 0, transform.scale);
//        });
//
//        get_world().system<ray::Camera3D>().kind(fox::game::CameraBegin)
//            .each([](Entity e, ray::Camera3D& camera)
//        {
//            // camera.Update();
//            camera.BeginMode();
//            e.get_world()->run_phase(fox::game::CameraDraw);
//            camera.EndMode();
//        });
//
//        get_world().system<ray::Cube, fox::Transform>().kind(fox::game::CameraDraw)
//            .each([](Entity e, ray::Cube& cube, fox::Transform& transform)
//        {
//            cube.Draw(transform.position, transform.scale);
//        });
//
//        get_world().system<ray::Texture, fox::Transform>().kind(fox::ecs::OnStore)
//            .each([](Entity e, ray::Texture& texture, fox::Transform& transform)
//                  {
//                      ray::Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
//                      ray::Rectangle destRec = { transform.position.x, transform.position.y, texture.width * transform.scale.x, texture.height * transform.scale.y };
//                      texture.Draw(sourceRec, destRec, Vec2(), transform.rotation.GetAngle());
//                  });
//
//        get_world().system<ray::Rectangle, fox::Transform>().kind(fox::ecs::PreStore)
//            .each([](Entity e, ray::Rectangle& rec, fox::Transform& transform)
//        {
//            rec.SetPosition(transform.position.x, transform.position.y);
//        });
//
//        get_world().system<ray::Rectangle>().kind(fox::ecs::OnStore)
//                .each([](Entity e, ray::Rectangle& rec)
//                      {
//                        rec.Draw();
//                      });
//
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
//
//        get_world().system<fox::Image, fox::Transform>().kind(ecs::OnStore)
//            .each([](Entity e, fox::Image& img, fox::Transform& transform)
//                  {
//                      ray::Rectangle sourceRec = { 0.0f, 0.0f, (float)img.sprite().width, (float)img.sprite().height };
//                      ray::Rectangle destRec = { transform.position.x, transform.position.y, img.sprite().width * transform.scale.x, img.sprite().height * transform.scale.y };
//                      img.sprite().Draw(sourceRec, destRec, Vec2(), transform.rotation.GetAngle());
//                  });
//
//        get_world().system<ray::Text, fox::Transform>().kind(fox::ecs::OnStore)
//            .each([](Entity e, ray::Text& text, fox::Transform& transform)
//        {
//            text.Draw(transform.position.x, transform.position.y);
//        });
//
//        get_world()
//                .system<RigidBody, fox::Transform>()
//                .kind(fox::ecs::PreFixUpdate)
//                .each([&](
//                        Entity e, RigidBody &body,
//                        fox::Transform &transform)
//                      {
//                          body.get_rb()->setTransform(transform);
//                      });
//        get_world()
//                .system<RigidBody, fox::Transform>()
//                .kind(fox::ecs::PostFixUpdate)
//                .each([&](
//                        Entity e, RigidBody &body,
//                        fox::Transform &transform)
//                      {
//                          const reactphysics3d::Transform &qTrans =
//                                  body.get_rb()->getTransform();
//                          transform.position =
//                                  Vec3(qTrans.getPosition().x, qTrans.getPosition().y,
//                                       qTrans.getPosition().z);
//                      });
//
//        get_world().system<RigidBody, BoxCollider, fox::Transform>()
//                .kind(ecs::OnAdd)
//                .each([](Entity e, RigidBody& body, BoxCollider& shape, fox::Transform& transform)
//                      {
//                          reactphysics3d::Vector3 v = shape.getReactShape()->getHalfExtents();
//                          fox::Transform trans = fox::Transform();
//                          trans.scale = Vec3(v.x * (transform.scale.x * .5f), v.y * (transform.scale.y * .5f), v.z * (transform.scale.z * .5f));
//                          shape.getReactShape()->setHalfExtents(shape.getHalfExtents() * trans.scale);
//                          auto f = body.get_rb()->addCollider(shape.getReactShape(), trans);
//                          f->getMaterial().setBounciness(0);
//                          shape.set_collider(f);
//                          // f->getMaterial().setFrictionCoefficient(0);
//                          // f->getMaterial().setMassDensity(0);
//                          // f->getMaterial().setRollingResistance(0);
//                          f->setUserData(&shape);
//                      });
//        get_world().system<RigidBody, BoxCollider>()
//                .kind(ecs::OnRemove)
//                .each([](Entity e, RigidBody& body, BoxCollider& shape)
//                      {
//                          body.get_rb()->removeCollider(shape.get_collider());
//                      });
//
//        get_world().system<ray::InputBox, fox::Transform>().kind(ecs::OnStore)
//            .each([](Entity e, ray::InputBox& box, fox::Transform& transform)
//                  {
//                      box.Draw();
//                  });
//    }

    Entity Scene::NewEntity(const std::string &name)
    {
        Entity e = m_oWorld.new_entity(name);
        e.add<TransformComponent>();
        return e;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_oWorld.delete_entity(entity.get_id());
    }

    void Scene::OnUpdateEditor(EditorCamera &camera)
    {
        Renderer2D::BeginScene(camera);

        auto view = m_oWorld.get_entities_with<TransformComponent, SpriteRenderer>();
        for (auto entity : view)
        {
            auto& transform = entity.get<TransformComponent>().value();
            auto& sprite = entity.get<SpriteRenderer>().value();

            Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
        }

        Renderer2D::EndScene();
    }

    void Scene::OnUpdateRuntime()
    {
        m_oWorld.run_phase(ecs::PreUpdate);
        m_oWorld.run_phase(ecs::OnUpdate);
        m_oWorld.run_phase(ecs::OnValidate);
        m_oWorld.run_phase(ecs::PostUpdate);
        m_oWorld.run_phase(ecs::PreStore);
        // m_oWorld.deleted_entities();

        // Render 2D
        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        {
             auto view = m_oWorld.get_entities_with<TransformComponent, CameraComponent>();
             for (auto entity : view)
             {
                 auto& transform = entity.get<TransformComponent>().value();
                 auto& camera = entity.get<CameraComponent>().value();

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
             m_oWorld.run_phase(ecs::OnStore);
             Renderer2D::EndScene();
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportHeight = height;
        m_ViewportWidth = width;

        auto view = m_oWorld.get_entities_with<CameraComponent>();
        for (auto e : view)
        {
            auto& cameraComponent = e.get<CameraComponent>().value();
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.camera.SetViewportSize(width, height);
        }
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_oWorld.get_entities_with<CameraComponent>();
        for (auto e : view)
        {
            auto& cameraComponent = e.get<CameraComponent>().value();
            if (cameraComponent.Primary)
                return e;
        }
        return {};
    }

    World &Scene::GetWorld()
    {
        return m_oWorld;
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

    template<>
    void Scene::OnComponentAdded<EntityTag>(Entity &e, EntityTag& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity &e, CameraComponent& component)
    {
        component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
}// namespace fox