#pragma once

#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "DynamicArray.hpp"
#include "common.hpp"

class EntityManager
{
public:
    using Ptr = std::unique_ptr<EntityManager>;

	/**
	 * @brief Construct a new Entity Manager object
	 */
	EntityManager()
	{
		for (EntityId entity = 0; entity < 10; ++entity)
		{
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
		
		EntityId id = mAvailableEntities.back();
		mAvailableEntities.pop();
		++mLivingEntityCount;
		m_vEntity.push(id);

		return id;
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
		m_vEntity.remove(entity);
		--mLivingEntityCount;
	}

	/**
	 * @brief Set the Signature of the entity pass in parameter
	 * 
	 * @param entity the entity
	 * @param signature the signature to add in the entity
	 */
	void SetSignature(EntityId entity, const CompSignature& signature)
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
	CompSignature& GetSignature(EntityId entity)
	{
		// assert(entity < MAX_ENTITIES && "Entity out of range.");

		return m_vSignatures[entity];
	}

public:
	DynamicArray<EntityId> mAvailableEntities;
	std::unordered_map<EntityId, CompSignature> m_vSignatures;
	DynamicArray<EntityId> m_vEntity;
	uint32_t mLivingEntityCount;
};