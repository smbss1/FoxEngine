/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** World.cpp
*/

#include <algorithm>
#include <iostream>
#include "Ecs/World.hpp"
#include "Components/EntityName.hpp"

namespace fox
{
    World::World()
    {
        m_pSysManager = std::make_unique<SystemManager>();
        m_pCompManager = std::make_unique<ComponentManager>();
        m_pEntityManager = std::make_unique<EntityManager>();
        m_pPipeline = set<Pipeline::SPtr>(new Pipeline);
        add_phase(fox::ecs::PreUpdate);
        add_phase(fox::ecs::OnUpdate);
        add_phase(fox::ecs::OnValidate);
        add_phase(fox::ecs::PostUpdate);
        add_phase(fox::ecs::PreStore);
        add_phase(fox::ecs::OnStore);
    }

    World::~World()
    {

    }

    Entity World::new_entity()
    {
        return new_entity("", true);
    }

    Entity World::new_entity(const std::string &name)
    {
        return new_entity(name, true);
    }

    Entity World::new_entity(const std::string &name, const std::string &tag_name)
    {
        return new_entity(name, tag_name, true);
    }

    Entity World::new_entity(const std::string &name, bool enable)
    {
        EntityId ent = m_pEntityManager->CreateEntity();
        Entity e = Entity {this, ent};
        e.set_enable(enable);
        if (!name.empty())
            e.add<fox::EntityName>(name);
        else
            e.add<fox::EntityName>("GameObject");
        return e;
    }

    Entity World::new_entity(const std::string &name, const std::string &tag_name, bool enable)
    {
        Entity e = new_entity(name, enable);
        if (!tag_name.empty())
            e.add<fox::EntityTag>(tag_name);
        else
            e.add<fox::EntityTag>("Undefined");
        return e;
    }

    bool World::get_entity(const std::string &name, Entity& e)
    {
        EntityId ent = -1;
        m_pEntityManager->ForEach([&](EntityId eId) -> bool
          {
              fox::Option<fox::EntityName &> ent_name = get_component<fox::EntityName>(eId);
              if (ent_name && ent_name->get() == name) {
                  ent = eId;
                  return true;
              }
              return false;
          });
        if (ent != -1) {
            e = Entity {this, ent};
            return true;
        }
        return false;
    }

    bool World::get_entity_with_tag(const std::string &tag_name, Entity& e)
    {
        EntityId ent = -1;
        m_pEntityManager->ForEach([&](EntityId eId) -> bool
          {
              fox::Option<fox::EntityTag&> ent_tag = get_component<fox::EntityTag>(eId);
              if (ent_tag && ent_tag->get() == tag_name) {
                  ent = eId;
                  return true;
              }
              return false;
          });
        if (ent != -1) {
            e = Entity {this, ent};
            return true;
        }
        return false;
    }

    bool World::get_entities_with_tag(const std::string &tag_name, std::vector<fox::Entity>& vE)
    {
        std::vector<fox::Entity>& ent = vE;
        ent.clear();
        m_pEntityManager->ForEach([&](EntityId eId) -> bool
          {
              fox::Option<fox::EntityTag&> ent_tag = get_component<fox::EntityTag>(eId);
              if (ent_tag && ent_tag->get() == tag_name) {
                  ent.push_back({this, eId});
              }
              return false;
          });
        return !ent.empty();
    }

    DynamicArray<EntityId>& World::get_entities() const
    {
        return m_pEntityManager->m_vEntity;
    }

    void World::clear_entities()
    {
        m_pCompManager->Clear();
        m_pSysManager->Clear();
        m_pEntityManager->DestroyAllEntity();
    }

    void World::delete_entity(EntityId e)
    {
        m_vRemoved.push_back(e);
    }

    void World::deleted_entities()
    {
        auto copy = m_vRemoved;
        m_vRemoved.clear();
        for (auto e : copy) {
            auto &signature = m_pEntityManager->GetSignature(e);
            signature.reset();
            signature.set(EcsDisable, true);
            m_pSysManager->EntitySignatureChanged(e, signature);
            m_pCompManager->EntityDestroyed(e);
            m_pEntityManager->DestroyEntity(e);
        }
    }
}