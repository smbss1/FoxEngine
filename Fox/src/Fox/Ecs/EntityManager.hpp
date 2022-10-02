/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** EntityManager.hpp
*/

#pragma once

#include <array>
#include <cassert>
#include <iostream>
#include <queue>
#include <memory>
#include <unordered_map>
#include "Utils/DynamicArray.hpp"
#include "common.hpp"

namespace fox
{
    class EntityManager
    {
    public:
        using Ptr = std::unique_ptr<EntityManager>;

        /**
         * @brief Construct a new Entity Manager object
         */
        EntityManager()
        {
            for (EntityId entity = 0; entity < 1000; ++entity) {
                mAvailableEntities.push(entity);
            }
        }

        /**
         * @brief Create an Entity
         *
         * @return EntityId the id of the Entity
         */
        EntityId CreateEntity()
        {
            // assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

            EntityId id = mAvailableEntities.front();
            mAvailableEntities.pop();
            ++mLivingEntityCount;
            m_vEntity.push_back(id);
            m_vSignatures.emplace(id, std::move(bitset()));

            return id;
        }

        /**
         * @brief Perform a foreach in all entity
         * @param fn the function called, if it's return true the loop continue otherwise it's break
         */
        void ForEach(std::function<bool(EntityId)> fn)
        {
            for (int i = 0; i < m_vEntity.size(); ++i) {
                if (fn(m_vEntity[i]))
                    break;
            }
        }

        /**
         * @brief Remove the entity to the list
         *
         * @param entity the entity to remove
         */
        void DestroyEntity(EntityId entity)
        {
            // assert(entity < MAX_ENTITIES && "Entity out of range.");

            m_vSignatures[entity].reset();
            mAvailableEntities.push(entity);
            m_vEntity.erase(std::remove(m_vEntity.begin(), m_vEntity.end(), entity));
            --mLivingEntityCount;
        }

        /**
         * @brief Clear all entities
         */
        void DestroyAllEntity()
        {
            m_vSignatures.clear();
            m_vEntity.clear();
            mLivingEntityCount = 0;
            while (!mAvailableEntities.empty())
                mAvailableEntities.pop();

            for (EntityId entity = 0; entity < 1000; ++entity) {
                mAvailableEntities.push(entity);
            }
        }

        /**
         * @brief Enable an entity
         * @param eId the entity id
         * @param v the enable value
         */
        void enable_entity(EntityId eId, bool v)
        {
            CompSignature &sig = GetSignature(eId);
            sig.set(fox::EcsDisable, v);
        }

        /**
         * @brief Check if an entity is enbale or not
         * @param eId the entity id
         * @return true if entity is enable otherwise false
         */
        bool is_entity_enable(EntityId eId)
        {
            CompSignature &sig = GetSignature(eId);
            return sig.test(fox::EcsDisable);
        }

        /**
         * @brief Set the Signature of the entity pass in parameter
         *
         * @param entity the entity
         * @param signature the signature to add in the entity
         */
        void SetSignature(EntityId entity, const CompSignature &signature)
        {
            // assert(entity < MAX_ENTITIES && "Entity out of range.");

            m_vSignatures[entity] = signature;
        }

        /**
         * @brief Get the Signature of the entity pass in parameter
         *
         * @param entity the entity
         * @return CompSignature& a reference to the signature
         */
        CompSignature &GetSignature(EntityId entity)
        {
            // assert(entity < MAX_ENTITIES && "Entity out of range.");

            return m_vSignatures[entity];
        }

    public:
        std::queue<EntityId> mAvailableEntities;
        std::unordered_map<EntityId, CompSignature> m_vSignatures;
        std::vector<EntityId> m_vEntity;
        uint32_t mLivingEntityCount;
    };
}