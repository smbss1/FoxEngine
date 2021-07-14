/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Vec2.hpp
*/

#ifndef FOX_VEC2_HPP_
#define FOX_VEC2_HPP_

#include <math.h>
#include "Vec3.hpp"

namespace fox
{
    class Vec2
    {
    public:
        float x;
        float y;

        Vec2() : x(0), y(0)
        {}

        Vec2(float _x, float _y) : x(_x), y(_y)
        {}

        /**
         * @brief Set the X and Y value
         * @param x_ the new X value
         * @param y_ the new Y value
         */
        void set(float x_, float y_)
        {
            x = x_;
            y = y_;
        }

        operator Vec3() const
        {
            return Vec3(x, y, 0);
        }

        Vec2 operator-()
        { return Vec2(-x, -y); }

        Vec2 &operator=(const Vec2 &v) = default;

        void operator+=(const Vec2 &v)
        {
            x += v.x;
            y += v.y;
        }

        void operator-=(const Vec2 &v)
        {
            x -= v.x;
            y -= v.y;
        }

        void operator*=(float a)
        {
            x *= a;
            y *= a;
        }

        /**
         * @brief Get the length
         * @return the length
         */
        float Length() const
        {
            return sqrtf(x * x + y * y);
        }
    };

    Vec2 operator+(const Vec2 &l, const Vec2 &r);

    Vec2 operator-(const Vec2 &l, const Vec2 &r);

    Vec2 operator*(const Vec2 &v, float f);

    Vec2 operator*(const Vec2 &l, const Vec2 &r);
}

#endif