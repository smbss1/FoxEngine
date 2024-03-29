//
// Created by samuel on 02/10/22.
//

#ifndef FOX_ENGINE_IDCOMPONENT_HPP
#define FOX_ENGINE_IDCOMPONENT_HPP

#include "Core/UUID.hpp"

namespace fox
{
    struct IDComponent
    {
        UUID ID;

        IDComponent() = default;
        IDComponent(UUID id)
        {
            ID = id;
        }
        IDComponent(const IDComponent&) = default;
    };

    struct PrefabComponent
    {
        UUID PrefabID;
        UUID EntityID;

        PrefabComponent() = default;
        PrefabComponent(const PrefabComponent&) = default;
    };
}

#endif //FOX_ENGINE_IDCOMPONENT_HPP
