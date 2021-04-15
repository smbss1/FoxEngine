
#include <algorithm>
#include <iostream>
#include "World.hpp"

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
    EntityId ent = m_pEntityManager->CreateEntity();
    return { this, ent };
}

void World::delete_entity(EntityId e)
{
    m_pEntityManager->DestroyEntity(e);
}


Any& World::set_resource(std::type_index id, Any any)
{
    m_vResources.erase(id);
    m_vResources.insert(std::make_pair(id, std::move(any)));
    return m_vResources.at(id);
}

fox::Option<Any&> World::get_resource(std::type_index id)
{
    auto it = m_vResources.find(id);

    if (it != m_vResources.end()) {
        return fox::just(it->second);
    } else
        return {};
}

fox::Option<const Any&> World::get_resource(std::type_index id) const
{
    auto it = m_vResources.find(id);

    if (it != m_vResources.end()) {
        return fox::just(it->second);
    } else
        return {};
}

fox::Option<Any> World::remove_resource(std::type_index id)
{
    auto it = m_vResources.find(id);

    if (it != m_vResources.end()) {
        auto any = std::move(it->second);
        m_vResources.erase(id);
        return fox::just(std::move(any));
    } else
        return {};
}
// void World::add_entity_to_archetype(EntityId e)
// {
//     // Type& type = m_pRoot->type;
//     // for (int y = 0; y < type.size(); ++y) {
//     // Archetype *node = m_pRoot.get();
//     // for (int i = 0; i < type.size(); ++i) {
//     //     Edge* edge = nullptr;
//     //     try {
//     //         std::cout << "at T: " << type[y] << std::endl;
//     //         std::cout << node->edges.size() << std::endl;
//     //         edge = &node->edges.at(type[y]);
//     //         node = edge->add.get();
//     //     } catch(const std::out_of_range& ex) {
//     //         Edge ed = Edge();

//     //         std::cout << "Create a new archetype of type: " << type[y] << std::endl;
            
//     //         ed.add = std::make_unique<Archetype>();
//     //         // for (auto i : node->type) {
//     //         //     std::cout << "Type: " << i << std::endl;
//     //         //     ed.add->type.push_back(i);
//     //         // }
//     //         ed.add->type.push_back(type[y]);
//     //         ed.add->entity_ids.push_back(e);

//     //         // Record& record = m_vEntities[e];

//     //         // if (record.archetype) {
//     //         //     // ComponentArray::Ptr array = std::make_unique<ComponentArray>();
//     //         //     // edge->add->components.push_back()
//     //         //     for (auto comp : record.archetype->components) {
//     //         //         edge->add->components.push_back(comp);
//     //         //     }
//     //         // }
//     //         node->edges.emplace(type[y], std::move(ed));
//     //         edge = &node->edges[type[y]];
//     //     }
//     // }
//     // }
// }
