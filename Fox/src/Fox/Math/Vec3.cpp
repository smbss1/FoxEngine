/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Vec3.cpp
*/

#include <Utils/Vec2.hpp>
#include "Utils/Vec3.hpp"

namespace fox
{
    Vec2 operator+(const Vec2 &l, const Vec2 &r)
    {
        return Vec2(l.x + r.x, l.y + r.y);
    }

    Vec2 operator-(const Vec2 &l, const Vec2 &r)
    {
        return Vec2(l.x - r.x, l.y - r.y);
    }

    Vec2 operator*(const Vec2 &v, float f)
    {
        return Vec2(v.x * f, v.y * f);
    }

    Vec2 operator*(const Vec2 &l, const Vec2 &r)
    {
        return Vec2(l.x * r.x, l.y * r.y);
    }

    Vec3 operator+(const Vec3 &l, const Vec3 &r)
    {
        return Vec3(l.x + r.x, l.y + r.y, l.z + r.z);
    }

    Vec3 operator-(const Vec3 &l, const Vec3 &r)
    {
        return Vec3(l.x - r.x, l.y - r.y, l.z - r.z);
    }

    Vec3 operator*(const Vec3 &v, float f)
    {
        return Vec3(v.x * f, v.y * f, v.z * f);
    }

    Vec3 operator*(const Vec3 &l, const Vec3 &r)
    {
        return Vec3(l.x * r.x, l.y * r.y, l.z * r.z);
    }

    float dot(const Vec3 &l, const Vec3 &r)
    {
        return l.x * r.x + l.y * r.y + l.z * r.z;
    }

    float angle(const Vec3 &l, const Vec3 &r)
    {
        float sqMagL = l.x * l.x + l.y * l.y + l.z * l.z;
        float sqMagR = r.x * r.x + r.y * r.y + r.z * r.z;

        if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON) {
            return 0.0f;
        }

        float dot = l.x * r.x + l.y * r.y + l.z * r.z;
        float len = sqrtf(sqMagL) * sqrtf(sqMagR);
        return acosf(dot / len);
    }

    Vec3 project(const Vec3 &a, const Vec3 &b)
    {
        float magBSq = b.Length();
        if (magBSq < VEC3_EPSILON) {
            return Vec3();
        }
        float scale = dot(a, b) / magBSq;
        return b * scale;
    }

    Vec3 reject(const Vec3 &a, const Vec3 &b)
    {
        Vec3 projection = project(a, b);
        return a - projection;
    }

    Vec3 reflect(const Vec3 &a, const Vec3 &b)
    {
        float magBSq = b.Length();
        if (magBSq < VEC3_EPSILON) {
            return Vec3();
        }
        float scale = dot(a, b) / magBSq;
        Vec3 proj2 = b * (scale * 2);
        return a - proj2;
    }

    Vec3 cross(const Vec3 &l, const Vec3 &r)
    {
        return Vec3(
                l.y * r.z - l.z * r.y,
                l.z * r.x - l.x * r.z,
                l.x * r.y - l.y * r.x
        );
    }

    Vec3 lerp(const Vec3 &s, const Vec3 &e, float t)
    {
        return Vec3(
                s.x + (e.x - s.x) * t,
                s.y + (e.y - s.y) * t,
                s.z + (e.z - s.z) * t
        );
    }

    Vec3 slerp(const Vec3 &s, const Vec3 &e, float t)
    {
        if (t < 0.01f) {
            return lerp(s, e, t);
        }

        Vec3 from = s.Normalized();
        Vec3 to = e.Normalized();

        float theta = angle(from, to);
        float sin_theta = sinf(theta);

        float a = sinf((1.0f - t) * theta) / sin_theta;
        float b = sinf(t * theta) / sin_theta;

        return from * a + to * b;
    }

    Vec3 nlerp(const Vec3 &s, const Vec3 &e, float t)
    {
        Vec3 linear(
                s.x + (e.x - s.x) * t,
                s.y + (e.y - s.y) * t,
                s.z + (e.z - s.z) * t
        );
        return linear.Normalized();
    }

    bool operator==(const Vec3 &l, const Vec3 &r)
    {
        Vec3 diff(l - r);
        return diff.LengthSquare() < VEC3_EPSILON;
    }

    bool operator!=(const Vec3 &l, const Vec3 &r)
    {
        return !(l == r);
    }

    std::ostream &operator<<(std::ostream &os, const Vec3 &v)
    {
        os << "(" << v.x << "," << v.y << "," << v.z << ")";
        return os;
    }
}