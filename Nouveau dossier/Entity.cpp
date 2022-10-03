/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Entity.cpp
*/

#include "Ecs/World.hpp"

namespace fox
{
    bool Entity::operator==(const Entity &e) const
    {
        return m_iId == e.get_id() && &m_oWld == &e.m_oWld;
    }

    bool Entity::operator!=(const Entity &other) const
    {
        return !(*this == other);
    }

    void Entity::destroy()
    {
        m_oWld->delete_entity(m_iId);
    }
}