

#ifndef FOX_TRANSFORM_HPP_
#define FOX_TRANSFORM_HPP_

#include "../Vec2.hpp"

struct Transform
{
    Vec2 position;
    Vec2 rotation;
    Vec2 scale;

    Transform* parent;

    Vec2 get_pos() const
    {
        return position;
    }

    Vec2 get_rot() const
    {
        return position;
    }

    Vec2 get_scale() const
    {
        return position;
    }

    void set_pos(Vec2 v)
    {
        position = v;
    }

    void set_pos(float x, float y)
    {
        position.x = x;
        position.y = y;
    }
};

#endif