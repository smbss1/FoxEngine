/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** Vec2.hpp header
*/

#ifndef FOX_VEC2_HPP_
#define FOX_VEC2_HPP_

#include <math.h>

// namespace fox
// {
	class Vec2
	{
	public:
		Vec2() {}
		Vec2(float x, float y) : x(x), y(y) {}

		void set(float x_, float y_)
		{
			x = x_;
			y = y_;
		}

		Vec2 operator -() { return Vec2(-x, -y); }
		
		void operator += (const Vec2& v)
		{
			x += v.x;
			y += v.y;
		}
		
		void operator -= (const Vec2& v)
		{
			x -= v.x;
			y -= v.y;
		}

		void operator *= (float a)
		{
			x *= a;
			y *= a;
		}

		float Length() const
		{
			return sqrtf(x * x + y * y);
		}

		float x;
		float y;
	};
// } // namespace fox

#endif