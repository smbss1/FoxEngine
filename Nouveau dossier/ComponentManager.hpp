/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** ComponentManager.hpp
*/

#pragma once

#include <memory>
#include <unordered_map>
#include <Components/Component.hpp>
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

            auto pArray = std::make_shared<ComponentArray>();
            if (!pArray)
                throw std::runtime_error("Component Array allocation failed.");
            m_vComponentTypes.emplace(typeid(T).hash_code(), m_uNextComponentType);
//            m_vComponentArrays.emplace(typeid(T).hash_code(), pArray);
            m_vComponentArrays.push_back(pArray);

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
            return m_vComponentTypes[typeid(T).hash_code()];
        }

        CompId GetComponentType(ref<Component> pComponent)
        {
            return m_vComponentTypes[pComponent->Id()];
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
            m_vComponentArrays[GetComponentType<T>()]->template Add<T>(entity, std::forward<Args>(args)...);

//            GetComponentArray<T>()->InsertData(entity);
            return *m_vComponentArrays[GetComponentType<T>()]->template Get<T>(entity);
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
            m_vComponentArrays[GetComponentType<T>()]->template Add<T>(entity);

//            GetComponentArray<T>()->InsertData(entity);
            return *m_vComponentArrays[GetComponentType<T>()]->template Get<T>(entity);
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
            m_vComponentArrays[GetComponentType<T>()]->Remove(entity);
        }

        void RemoveComponent(EntityId entity, ref<Component> pComponent)
        {
            m_vComponentArrays[GetComponentType(pComponent)]->Remove(entity);
        }

        /**
         * @brief Set component to the entity index, if the enity has not the component, it will be assign
         * @param entity the entity id
         * @param pComponent the component object
         */
        void SetComponent(EntityId entity, ref<Component> pComponent)
        {
            m_vComponentArrays[GetComponentType(pComponent)]->Set(entity, pComponent);
        }

        /**
         * @brief Clear the components arrays
         */
        void Clear()
        {
            for (auto it : m_vComponentArrays) {
//                it.second->clear();
                it->clear();
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
        fox::ref<T> GetComponent(EntityId entity)
        {
            return m_vComponentArrays[GetComponentType<T>()]->template Get<T>(entity);
        }

        /**
         * @brief Remove entity to all components array
         *
         * @param entity the index or id
         */
        void EntityDestroyed(EntityId entity)
        {
            for (auto const &pair : m_vComponentArrays) {
//                auto const &component = pair.second;

                pair->EntityDestroyed(entity);
            }
        }

        std::vector<ref<Component>> GetAllComponents(EntityId entityID, CompSignature entitySig)
        {
            std::vector<ref<Component>> vComponents;
            for (int i = 0; i < m_vComponentArrays.size(); ++i)
            {
                if (entitySig.test(i))
                {
                    vComponents.push_back(m_vComponentArrays[i]->Get(entityID));
                }
            }
            return vComponents;
        }

    private:
        std::unordered_map<std::size_t, CompId> m_vComponentTypes;
        std::vector<std::shared_ptr<ComponentArray>> m_vComponentArrays;

        CompId m_uNextComponentType = 0;
    };
}