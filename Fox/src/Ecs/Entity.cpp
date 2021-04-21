
#include "Ecs/World.hpp"

bool Entity::operator==(const Entity& e) const
{
    return m_iId == e.get_id() && &m_oWld == &e.m_oWld;
}

bool Entity::operator!=(const Entity& other) const
{
    return !(*this == other);
}

void Entity::destroy()
{
    m_oWld->delete_entity(m_iId);
}