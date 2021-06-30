/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** ComponentManager.hpp
*/

#pragma once

#include <memory>
#include <unordered_map>
#include "common.hpp"
#include "ComponentArray.hpp"

namespace fox
{
    class ComponentManager
    {
    public:
        using Ptr = std::unique_ptr<ComponentManager>;

        /**
         * @brief Register a Component to be used in the program
         *
         * @tparam T the type of the Component
         */
        template<typename T>
        void RegisterComponent()
        {
            if (ComponentIsRegistered<T>())
                throw std::runtime_error("Registering component type more than once.");

            auto pArray = std::make_shared<ComponentArray<T>>();
            if (!pArray)
                throw std::runtime_error("Component Array allocation failed.");
            m_vComponentTypes.emplace(typeid(T).hash_code(), m_uNextComponentType);
            m_vComponentArrays.emplace(typeid(T).hash_code(), pArray);

            ++m_uNextComponentType;
        }

        /**
         * @brief Get the Type(ID) of a Component
         *
         * @tparam T the type of the Component
         * @return CompId the id of the Component
         */
        template<typename T>
        CompId GetComponentType()
        {
            if (!ComponentIsRegistered<T>())
                RegisterComponent<T>();

            return m_vComponentTypes[typeid(T).hash_code()];
        }

        /**
         * @brief Get the size of registred components
         *
         * @return CompId the size of registred components
         */
        CompId GetSize()
        {
            return m_uNextComponentType;
        }

        /**
         * @brief Check the Component is register or not
         *
         * @tparam T the type of the Component
         * @return true if Component is register
         * @return false if it's not register
         */
        template<typename T>
        bool ComponentIsRegistered()
        {
            return m_vComponentTypes.find(typeid(T).hash_code()) != m_vComponentTypes.end();
        }

        /**
         * @brief Add component to the entity index
         *
         * @tparam T the type of the Component
         * @tparam Args the list of arguments for the constructor of the component
         * @param entity the entity id
         * @param args arguments for the constructor of the component
         * @return T& referenc to the Component
         */
        template<typename T, typename... Args>
        T &AddComponent(EntityId entity, Args &&... args)
        {
            if (!ComponentIsRegistered<T>())
                RegisterComponent<T>();
            GetComponentArray<T>()->InsertData(entity, std::forward<Args>(args)...);
            return GetComponentArray<T>()->GetData(entity).value();
        }

        /**
         * @brief Add component to the entity index
         * @tparam T the type of the Component
         * @param entity the entity id
         * @return T& referenc to the Component
         */
        template<typename T>
        T &AddComponent(EntityId entity)
        {
            if (!ComponentIsRegistered<T>())
                RegisterComponent<T>();
            GetComponentArray<T>()->InsertData(entity);
            return GetComponentArray<T>()->GetData(entity).value();
        }

        /**
         * @brief Remove a component to the entity id
         *
         * @tparam T the type of the Component
         * @param entity the entity id
         */
        template<typename T>
        void RemoveComponent(EntityId entity)
        {
            if (ComponentIsRegistered<T>())
                GetComponentArray<T>()->RemoveData(entity);
        }

        /**
         * @brief Clear the components arrays
         */
        void Clear()
        {
            for (auto it : m_vComponentArrays) {
                it.second->clear();
            }
        }

        /**
         * @brief Get the component at the entity id
         *
         * @tparam T the type of the Component
         * @param entity the entity id
         * @return fox::Option<T&> an optionnal value
         */
        template<typename T>
        fox::Option<T&> GetComponent(EntityId entity)
        {
            assert(ComponentIsRegistered<T>());
            return GetComponentArray<T>()->GetData(entity);
        }

        /**
         * @brief Remove entity to all components array
         *
         * @param entity the index or id
         */
        void EntityDestroyed(EntityId entity)
        {
            for (auto const &pair : m_vComponentArrays) {
                auto const &component = pair.second;

                component->EntityDestroyed(entity);
            }
        }

    private:
        std::unordered_map<std::size_t, CompId> m_vComponentTypes;
        std::unordered_map<std::size_t, std::shared_ptr<IComponentArray>> m_vComponentArrays;
        CompId m_uNextComponentType = 1;

        /**
         * @brief Get the Component Array object
         *
         * @tparam T the type of the Component
         * @return std::shared_ptr<ComponentArray<T>> a pointer of the Array of the matched Type T
         */
        template<typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray()
        {
            return std::static_pointer_cast<ComponentArray<T>>(m_vComponentArrays[typeid(T).hash_code()]);
        }
    };
}