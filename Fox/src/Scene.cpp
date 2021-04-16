#include "Scene.hpp"
#include "Components/NativeScript.hpp"

namespace fox
{
    Scene::Scene()
    {
        
    }

    Scene::~Scene()
    {
        
    }

    void Scene::enable()
    {
        m_oWorld.run_phase(game::OnStart);
        on_enable();
    }
    void Scene::on_enable() { }


    void Scene::disable()
    {
        on_disable();
    }
    void Scene::on_disable() { }

    void Scene::update(Timestep dt)
    {
        m_oWorld.run_phase(ecs::PreUpdate);
        m_oWorld.run_phase(ecs::OnUpdate);
        on_update(dt);
        m_oWorld.run_phase(ecs::OnValidate);
        m_oWorld.run_phase(ecs::PostUpdate);
        m_oWorld.run_phase(ecs::PreStore);
        m_oWorld.run_phase(ecs::OnStore);
    }

    void Scene::init_systems()
    {
        m_oWorld.system<NativeScript>().kind(ecs::OnUpdate)
            .each([](Entity e, NativeScript& script) {
                if (!script.m_pInstance) {
                    script.m_pInstance = script.InstantiateScript();
                    script.m_pInstance->m_pEntity = e;
                    script.m_pInstance->OnCreate();
                }
                script.m_pInstance->OnUpdate();
        });
    }

    void Scene::destroy_systems()
    {
    }
}