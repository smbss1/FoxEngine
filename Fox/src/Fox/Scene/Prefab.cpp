//
// Created by samuel on 22/10/22.
//

#include "Prefab.hpp"

#include "Scene.hpp"
#include "Asset/AssetImporter.hpp"
#include "Components/IDComponent.hpp"
#include "Components/ScriptComponent.hpp"

namespace fox
{
    Entity Prefab::CreatePrefabFromEntity(Entity entity)
    {
        FOX_CORE_ASSERT(Handle);

        Entity newEntity = m_Scene->NewEntity();

        // Add PrefabComponent
        newEntity.add<PrefabComponent>(Handle, newEntity.get<IDComponent>().ID);

        entity.m_Scene->CopyAllComponentsIfExists(newEntity, entity);

//        for (auto childId : entity.Children())
//        {
//            Entity childDuplicate = CreatePrefabFromEntity(entity.m_Scene->GetEntityWithUUID(childId));
//
//            childDuplicate.SetParentUUID(newEntity.GetUUID());
//            newEntity.Children().push_back(childDuplicate.GetUUID());
//        }

//        if (newEntity.has<ScriptComponent>())
//            ScriptEngine::DuplicateScriptInstance(entity, newEntity);

        return newEntity;
    }

    Prefab::Prefab()
    {
        m_Scene = Scene::CreateEmpty();
        m_Scene->m_Registry.on_construct<ScriptComponent>().connect<&Prefab::OnScriptComponentConstruct>(this);
        m_Scene->m_Registry.on_destroy<ScriptComponent>().connect<&Prefab::OnScriptComponentDestroy>(this);
    }

    Prefab::~Prefab()
    {
        m_Scene->m_Registry.on_construct<ScriptComponent>().disconnect(this);
        m_Scene->m_Registry.on_destroy<ScriptComponent>().disconnect(this);
    }

    void Prefab::Create(Entity entity, bool serialize)
    {
//        if (m_Entity && m_Entity.has<ScriptComponent>())
//            ScriptEngine::ShutdownScriptEntity(m_Entity, true);

        m_Scene = Scene::CreateEmpty();
        m_Scene->m_Registry.on_construct<ScriptComponent>().connect<&Prefab::OnScriptComponentConstruct>(this);
        m_Scene->m_Registry.on_destroy<ScriptComponent>().connect<&Prefab::OnScriptComponentDestroy>(this);
        m_Entity = CreatePrefabFromEntity(entity);

        if (serialize)
            AssetImporter::Serialize(this);
    }

    void Prefab::OnScriptComponentConstruct(entt::registry& registry, entt::entity entity)
    {
//        auto entityID = registry.get<IDComponent>(entity).ID;
//        HZ_CORE_ASSERT(m_Scene->m_EntityMap.find(entityID) != m_Scene->m_EntityIDMap.end());
//        ScriptEngine::InitializeScriptEntity(m_Scene->m_EntityMap.at(entityID));
    }

    void Prefab::OnScriptComponentDestroy(entt::registry& registry, entt::entity entity)
    {
        if (registry.has<IDComponent>(entity))
        {
//            auto entityID = registry.get<IDComponent>(entity).ID;
//            FOX_CORE_ASSERT(m_Scene->m_EntityMap.find(entityID) != m_Scene->m_EntityMap.end());
//            ScriptEngine::ShutdownScriptEntity(m_Scene->m_EntityMap.at(entityID));
        }
    }
}
