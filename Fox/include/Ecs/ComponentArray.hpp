/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** ComponentArray.hpp
*/

#pragma once

#include "common.hpp"
#include <array>
#include <cassert>
#include <unordered_map>
#include <exception>
#include "Utils/Option.hpp"
#include <memory>

namespace fox
{
    class IComponentArray
    {
    public:
        virtual ~IComponentArray() = default;

        /**
         * @brief Called when a entity is destroyed
         *
         * @param entity the entity
         */
        virtual void EntityDestroyed(EntityId entity) = 0;

        /**
         * @brief Clear the component array
         */
        virtual void clear() = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray
    {
    public:
         /**
          * @brief Insert a new Component at the entity index
          * @tparam Args the Arguments types for constructor of the component
          * @param entity the index in the array
          * @param args the arguments for constructor of the component
          */
        template <typename... Args>
        inline void InsertData(EntityId entity, Args&&... args)
        {
            RemoveData(entity);
            m_vComponents.emplace(
                    std::piecewise_construct, std::forward_as_tuple(entity),
                    std::forward_as_tuple(std::forward<Args>(args)...));
        }

        /**
         * @brief Remove a Component at the entity index
         * @param entity the index
         */
        inline void RemoveData(EntityId entity)
        {
            auto it = m_vComponents.find(entity);
            if (it != m_vComponents.end()) {
                // delete it->second;
                m_vComponents.erase(it);
            }
        }

        /**
         * @brief Get the Component object at the entity index
         *
         * @param entity the index
         * @return fox::Option<T&> optionnal value
         */
        inline fox::Option<T&> GetData(EntityId entity)
        {
            auto it = m_vComponents.find(entity);

            if (it != m_vComponents.end())
                return just(it->second);
            return {};
        }

        /**
         * @brief Get the Component object at the entity index
         *
         * @param entity the index
         * @return fox::Option<T&> optionnal value
         */
        inline fox::Option<const T&> GetData(EntityId entity) const
        {
            auto it = m_vComponents.find(entity);

            if (it != m_vComponents.end())
                return just(it->second);
            return {};
        }

        /**
         * @brief Called when a entity is destroyed
         *
         * @param entity the entity
         */
        inline void EntityDestroyed(EntityId entity) override
        {
            RemoveData(entity);
        }

        /**
         * @brief Clear the component array
         */
        virtual void clear() override
        {
            if (m_vComponents.empty())
                return;
            m_vComponents.clear();
        }

    private:
        std::unordered_map<std::size_t, T> m_vComponents;
    };
}