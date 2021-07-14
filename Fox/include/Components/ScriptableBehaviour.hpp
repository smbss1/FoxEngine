/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** ScriptableBehaviour.hpp
*/

#ifndef ECS_SCRIPTABLEBEHAVIOUR_HPP
#define ECS_SCRIPTABLEBEHAVIOUR_HPP

#include <Scene/Scene.hpp>
#include "Utils/Option.hpp"
#include "Ecs/World.hpp"

namespace fox
{
    class Collider;
    class Application;
}

class ScriptableBehaviour
{
public:
    virtual ~ScriptableBehaviour() = default;

public:
    /**
     * @brief Called when the script is added in the entity
     */
    virtual void on_create() {}

    /**
     * @brief Called when the scene start
     */
    virtual void OnStart() {}

    /**
     * @brief Called every frame
     */
    virtual void on_update() {}

    /**
     * @brief Called when the script is removed to the entity or when the entity is destroyed
     */
    virtual void on_destroy() {}

    /**
     * @brief Called when a collision enter
     */
    virtual void on_collision_enter(fox::Collider&) {}

    /**
     * @brief Called when a collision stay
     */
    virtual void on_collision_stay(fox::Collider&) {}

    /**
     * @brief Called when a collision exit
     */
    virtual void on_collision_exit(fox::Collider&) {}

    /**
     * @brief Add a component to the entity
     *
     * @tparam T the type of the component
     * @tparam Args the list of arguments to pass to the constuctor of the component
     * @param args the arguments to pass to the constuctor of the component
     * @return T& a reference the component
     */
    template <typename T, typename... Args>
    T &add_component(Args &&...args)
    {
        assert(m_pWorld);
        return m_pWorld->template add_component<T>(m_eEntity, std::forward<Args>(args)...);
    }

    /**
     * @brief Add component to the entity index
     * @tparam T the type of the Component
     * @return T& reference to the Component
     */
    template <typename T>
    T &add_component()
    {
        assert(m_pWorld);
        return m_pWorld->template add_component<T>(m_eEntity);
    }

    /**
     * @brief Get the component object
     *
     * @tparam T the type of the component
     * @return fox::Option<T&> an optionnal value
     */
    template <typename T>
    fox::ref<T> get_component()
    {
        assert(m_pWorld);
        return m_pWorld->template get_component<T>(m_eEntity);
    }

    /**
     * @brief Set the id of the entity
     * @param id the id of the entity
     */
    void set_entity(EntityId id)
    {
        m_eEntity = id;
    }

    /**
     * @brief Set the world
     * @param world the ECS World
     */
    void set_world(fox::World& world)
    {
        m_pWorld = &world;
    }

    /**
     * @brief Set the application reference
     * @param app the application reference
     */
    void set_app(fox::Application& app);

    /**
     * @brief Set the scene reference
     * @param app the scene reference
     */
//    void set_scene(fox::Scene& scene)
//    {
//        m_pScene = &scene;
//    }

    /**
     * @brief Get the application
     * @return the application
     */
    fox::Application& get_app();

    /**
     * @brief Get the scene
     * @return the scene
     */
//    fox::Scene& get_scene()
//    {
//        return *m_pScene;
//    }

    /**
     * @brief Get the world
     * @return the world
     */
    fox::World& get_world()
    {
        return *m_pWorld;
    }

    /**
     * @brief Get the id of the entity
     * @return the id of the entity
     */
    [[nodiscard]] EntityId get_id() const
    {
        return m_eEntity;
    }

    /**
     * @brief Destroy the entity
     */
    void destroy()
    {
        assert(m_pWorld);
        m_pWorld->delete_entity(m_eEntity);
    }

    /**
     * @brief Create a new entity
     * @return the new created entity
     */
    fox::Entity new_entity()
    {
        assert(m_pWorld);
        return m_pWorld->new_entity();
    }

    fox::Entity new_entity(const std::string &name, bool enable = true)
    {
        assert(m_pWorld);
        return m_pWorld->new_entity(name, enable);
    }

    fox::Entity new_entity(const std::string &name, const std::string &tag, bool enable = true)
    {
        assert(m_pWorld);
        return m_pWorld->new_entity(name, tag, enable);
    }

private:
    EntityId m_eEntity{};
    fox::World* m_pWorld{};
    fox::Application* m_pApp{};
    fox::Scene* m_pScene{};
};

#endif //ECS_SCRIPTABLEBEHAVIOUR_HPP
