/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Quat.hpp
*/

#ifndef _H_QUAT_
#define _H_QUAT_

#include <cmath>
#include "Vec3.hpp"

namespace fox
{
    constexpr float QUAT_EPSILON = 0.000001f;

    struct quat
    {
        float x;
        float y;
        float z;
        float w;

        inline quat() :
                x(0), y(0), z(0), w(1)
        {}

        inline quat(float _x, float _y, float _z, float _w) :
                x(_x), y(_y), z(_z), w(_w)
        {}

        quat operator-() const
        { return quat(-x, -y, -z, -w); }

        void operator+=(const quat &q)
        {
            x += q.x;
            y += q.y;
            z += q.z;
            w += q.w;
        }

        void operator-=(const quat &q)
        {
            x -= q.x;
            y -= q.y;
            z -= q.z;
            w -= q.w;
        }

        void operator*=(float a)
        {
            x *= a;
            y *= a;
            z *= a;
        }

        /**
         * @brief Get the axis of the Quaternion
         * @return the axis of the Quaternion
         */
        Vec3 getAxis()
        {
            return Vec3(x, y, z).Normalized();
        }

        /**
         * @brief Get the angle of the Quaternion
         * @return the angle of the Quaternion
         */
        float GetAngle() const
        {
            return 2.0f * acosf(w);
        }

        /**
         * @brief Get the length square
         * @return the length square
         */
        float LengthSquare() const
        {
            return x * x + y * y + z * z + w * w;
        }

        /**
         * @brief Get the length
         * @return the length
         */
        float Length() const
        {
            float lenSq = LengthSquare();
            if (lenSq < QUAT_EPSILON)
                return 0.0f;
            return sqrtf(lenSq);
        }

        /**
         * @brief Normalize the Quaternion
         */
        void normalize()
        {
            float lenSq = LengthSquare();
            if (lenSq < QUAT_EPSILON) {
                return;
            }
            float i_len = 1.0f / sqrtf(lenSq);

            x *= i_len;
            y *= i_len;
            z *= i_len;
            w *= i_len;
        }

        /**
         * @brief Get the normalized value from this Quaternion
         * @return
         */
        quat normalized() const
        {
            float lenSq = LengthSquare();
            if (lenSq < QUAT_EPSILON)
                return quat();
            float i_len = 1.0f / sqrtf(lenSq);

            return quat(
                    x * i_len,
                    y * i_len,
                    z * i_len,
                    w * i_len
            );
        }

        /**
         * @brief Get the conjugate
         * @return the conjugate
         */
        quat conjugate() const
        {
            return quat(
                    -x,
                    -y,
                    -z,
                    w
            );
        }

        /**
         * @brief Get the invert
         * @return the invert
         */
        quat inverse() const
        {
            float lenSq = LengthSquare();
            if (lenSq < QUAT_EPSILON)
                return quat();
            float recip = 1.0f / lenSq;

            // conjugate / norm
            return quat(
                    -x * recip,
                    -y * recip,
                    -z * recip,
                    w * recip
            );
        }
    };

    quat angleAxis(float angle, const Vec3 &axis);

    quat fromTo(const Vec3 &from, const Vec3 &to);

    quat operator+(const quat &a, const quat &b);

    quat operator-(const quat &a, const quat &b);

    quat operator*(const quat &a, float b);

    bool operator==(const quat &left, const quat &right);

    bool operator!=(const quat &a, const quat &b);

    bool sameOrientation(const quat &left, const quat &right);

    float dot(const quat &a, const quat &b);

    quat operator*(const quat &Q1, const quat &Q2);

    quat operator*(const quat &Q1, const quat &Q2);

    Vec3 operator*(const quat &q, const Vec3 &v);

    quat mix(const quat &from, const quat &to, float t);

    quat nlerp(const quat &from, const quat &to, float t);

    quat operator^(const quat &q, float f);

    quat slerp(const quat &start, const quat &end, float t);

    quat lookRotation(const Vec3 &direcion, const Vec3 &up);
}

#endif