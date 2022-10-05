//
// Created by samuel on 05/07/2021.
//

#ifndef FOXENGINE_EDITOREVENT_HPP
#define FOXENGINE_EDITOREVENT_HPP

#include "Events/EventSystem.hpp"
#include "Ecs/Entity.hpp"

struct RuntimeStartEvent
{
};

struct RuntimeStopEvent
{
};

// Scene Hierarchy Events
struct OnSelectedEntityChangeEvent
{
    OnSelectedEntityChangeEvent(fox::Entity& entity) : m_oEntity(entity)
    {
    }
    fox::Entity& m_oEntity;
};

#endif //FOXENGINE_EDITOREVENT_HPP
