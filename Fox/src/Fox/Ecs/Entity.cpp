/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Entity.cpp
*/

#include "Ecs/Entity.hpp"
#include "Components/IDComponent.hpp"
#include "Components/EntityName.hpp"

namespace fox
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }

    UUID Entity::GetUUID() { return get<IDComponent>().ID; }
    const std::string& Entity::GetName() { return get<EntityName>().name; }
}
