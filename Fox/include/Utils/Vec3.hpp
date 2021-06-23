/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Vec3.hpp
*/

#ifndef FOX_VEC3_HPP_
#define FOX_VEC3_HPP_

#include <math.h>
#include <iostream>

constexpr float VEC3_EPSILON = 0.000001f;

// namespace fox
// {
	struct Vec3
	{
	    float x, y, z;

		Vec3() : x(0), y(0), z(0) {}
		Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        Vec3(Vec3&& other) = default;
		Vec3(const Vec3& other) = default;
		~Vec3() = default;

		Vec3& operator=(const Vec3& other)
        {
		    x = other.x;
		    y = other.y;
		    z = other.z;
            return *this;
        }

        /**
         * @brief Set the new XYZ value
         * @param x_ the new X value
         * @param y_ the new Y value
         * @param z_ the new Z value
         */
		void set(float x_, float y_, float z_)
		{
			x = x_;
			y = y_;
			z = z_;
		}

		Vec3 operator -() { return Vec3(-x, -y, -z); }

		void operator += (const Vec3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}

		void operator -= (const Vec3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		void operator *= (float a)
		{
			x *= a;
			y *= a;
			z *= a;
		}

        /**
         * @brief Get the square length
         * @return the square length
         */
		float LengthSquare() const
		{
			return x * x + y * y + z * z;
		}

        /**
         * @brief Get the length
         * @return the length
         */
		float Length() const
		{
			float lenSq = LengthSquare();
			if (lenSq < VEC3_EPSILON)
				return 0.0f;
			return sqrtf(lenSq);
		}

        /**
         * @brief Normalize the vector
         */
		void Normalize()
		{
			float lenSq = x * x + y * y + z * z;
			if (lenSq < VEC3_EPSILON)
				return;

			float invLen = 1.0f / sqrtf(lenSq);

			x *= invLen;
			y *= invLen;
			z *= invLen;
		}

        /**
         * @brief Get the normalized vector from this vector
         * @return the normalized vector from this vector
         */
		Vec3 Normalized() const
		{
			float lenSq = x * x + y * y + z * z;
			if (lenSq < VEC3_EPSILON)
				return *this;
			float invLen = 1.0f / sqrtf(lenSq);

			return Vec3(
				x * invLen,
				y * invLen,
				z * invLen
			);
		}
	};

	Vec3 operator+(const Vec3& l, const Vec3& r);
	Vec3 operator-(const Vec3& l, const Vec3& r);
	Vec3 operator*(const Vec3& v, float f);
	Vec3 operator*(const Vec3& l, const Vec3& r);
	float dot(const Vec3& l, const Vec3& r);
	void normalize(Vec3& v);
	Vec3 normalized(const Vec3& v);
	float angle(const Vec3& l, const Vec3& r);
	Vec3 project(const Vec3& a, const Vec3& b);
	Vec3 reject(const Vec3& a, const Vec3& b);
	Vec3 reflect(const Vec3& a, const Vec3& b);
	Vec3 cross(const Vec3& l, const Vec3& r);
	Vec3 lerp(const Vec3& s, const Vec3& e, float t);
	Vec3 slerp(const Vec3& s, const Vec3& e, float t);
	Vec3 nlerp(const Vec3& s, const Vec3& e, float t);
	bool operator==(const Vec3& l, const Vec3& r);
	bool operator!=(const Vec3& l, const Vec3& r);

	std::ostream& operator<<(std::ostream& os, const Vec3& v);
// } // namespace fox

#endif