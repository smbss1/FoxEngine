//
// Created by samuel on 05/07/2021.
//

#ifndef FOXENGINE_EDITOREVENT_HPP
#define FOXENGINE_EDITOREVENT_HPP

#include "Events/EventSystem.hpp"
#include "Ecs/Entity.hpp"

namespace fox
{
    // Scene Hierarchy Events
    struct OnSelectedEntityChangeEvent
    {
        OnSelectedEntityChangeEvent(fox::Entity& entity) : m_oEntity(entity)
        {
        }
        fox::Entity& m_oEntity;
    };

    struct OnContextChangeEvent
    {
        OnContextChangeEvent(Ref<fox::Scene> scene) : Context(scene)
        {
        }
        Ref<fox::Scene> Context;
    };

    struct OnProjectChangeEvent
    {
        OnProjectChangeEvent()
        {
        }
    };
}

#endif //FOXENGINE_EDITOREVENT_HPP
