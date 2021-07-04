/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Scene.hpp
*/

#ifndef FOX_SCENE_HPP_
#define FOX_SCENE_HPP_

#include "Ecs/World.hpp"
#include "Renderer/EditorCamera.hpp"

namespace fox
{
    class Application;
}

namespace fox
{
    class Scene
    {
    public:
        Scene(Application& app);
        ~Scene() = default;

        Entity NewEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        template<typename T, typename... Args>
        T &AddComponent(Entity e, Args&&... args)
        {
            return m_oWorld.template add_component<T>(e, args...);
        }

        template<typename T>
        T &AddComponent(Entity e)
        {
            T& comp = m_oWorld.template add_component<T>(e);
            OnComponentAdded(e, comp);
            return comp;
        }

        template<typename T>
        fox::Option<T&> GetComponent(Entity e)
        {
            return m_oWorld.template get_component<T>(e);
        }

        void OnStartRuntime();
        void OnUpdateRuntime();
        void OnUpdateEditor(EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);
//        void set_application(Application& app);

        Entity GetPrimaryCameraEntity();

        World& GetWorld();
        Application& GetApp();
    private:
        template<typename T>
        void OnComponentAdded(Entity &e, T &component);

    private:
        Application& m_oApp;
        World m_oWorld {};
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    protected:
        const float timeStep = 1.0f / 60.0f;
    };
}
#endif /* !SCENE_HPP_ */
