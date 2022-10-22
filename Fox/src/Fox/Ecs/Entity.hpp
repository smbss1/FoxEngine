/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** World.hpp
*/

#ifndef FOX_WORLD_HPP_
#define FOX_WORLD_HPP_

#include "Core/UUID.hpp"
#include "Scene/Scene.hpp"
#include "Core/Assert.hpp"

#include "entt.hpp"

namespace fox
{
    class EntitySerializer;
    class Prefab;
    class TransformComponent;
    class Entity
    {
    public:
        /**
         * @brief Construct a new Entity object
         */
        Entity() = default;

        /**
         * @brief Construct a new Entity object
         *
         * @param w the world
         * @param id the id of the entity
         */
        Entity(entt::entity handle, Scene* scene);


        /**
         * @brief Construct a new Entity object
         */
        Entity(const Entity& other) = default;

        operator bool () const { return m_EntityHandle != entt::null && m_Scene != nullptr; }
        operator entt::entity () const { return m_EntityHandle; }
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        UUID GetUUID();
        const std::string& GetName();
        TransformComponent& GetTransform();

        bool operator==(const Entity& other) const
        {
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

        Entity &operator=(const Entity &other)
        {
            m_EntityHandle = other.m_EntityHandle;
            m_Scene = other.m_Scene;
            return *this;
        }

        /**
         * @brief Add a component to the entity
         *
         * @tparam T the type of the component
         * @tparam Args the list of arguments to pass to the constuctor of the component
         * @param args the arguments to pass to the constuctor of the component
         * @return Enity& the class itself
         */
        template<typename T, typename... Args>
        T &add(Args &&... args)
        {
            FOX_ASSERT(!has<T>(), "Entity already has component!");
            T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
            return component;
        }

        template<typename T>
        T &add()
        {
            FOX_ASSERT(!has<T>(), "Entity already has component!");
            T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle);
            return component;
        }

        template<typename T, typename... Args>
        T& add_or_replace(Args&&... args)
        {
            T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
            return component;
        }

        /**
         * @brief Get the component object
         *
         * @tparam T the type of the component
         * @return fox::Option<T&> an optionnal value
         */
        template<typename T>
        T& get()
        {
            FOX_ASSERT(has<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template<typename T>
        bool has()
        {
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
        }

        /**
         * @brief remove a component to an entity
         *
         * @tparam T the type of the component to be remove
         */
        template<typename T>
        void remove()
        {
            FOX_ASSERT(has<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        Entity clone()
        {
            return m_Scene->CloneEntity(*this, {});
        }


    private:
        entt::entity m_EntityHandle { entt::null };
        Scene* m_Scene = nullptr;

        friend Scene;
        friend EntitySerializer;
        friend Prefab;
    };
}
#endif
