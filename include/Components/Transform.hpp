
#pragma once

#include "Math/Vec2.hpp"

struct Transform
{
    Vec2 position;
    Vec2 rotation;
    Vec2 scale;

    Transform* parent;
};