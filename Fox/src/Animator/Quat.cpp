/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Quat.cpp
*/

#include "Utils/Quat.hpp"
#include <iostream>

quat angleAxis(float angle, const Vec3 &axis)
{
	Vec3 norm = axis.Normalized();
	float s = sinf(angle * 0.5f);

	return quat(
		norm.x * s,
		norm.y * s,
		norm.z * s,
		cosf(angle * 0.5f));
}

quat fromTo(const Vec3 &from, const Vec3 &to)
{
	Vec3 f = from.Normalized();
	Vec3 t = to.Normalized();

	if (f == t)
	{
		return quat();
	}
	else if (f == t * -1.0f)
	{
		Vec3 ortho = Vec3(1, 0, 0);
		if (fabsf(f.y) < fabsf(f.x))
		{
			ortho = Vec3(0, 1, 0);
		}
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x))
		{
			ortho = Vec3(0, 0, 1);
		}

		Vec3 axis = cross(f, ortho).Normalized();
		return quat(axis.x, axis.y, axis.z, 0);
	}

	Vec3 half = (f + t).Normalized();
	Vec3 axis = cross(f, half);

	return quat(
		axis.x,
		axis.y,
		axis.z,
		dot(f, half));
}

quat operator+(const quat &a, const quat &b)
{
	return quat(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w);
}

quat operator-(const quat &a, const quat &b)
{
	return quat(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w);
}

quat operator*(const quat &a, float b)
{
	return quat(
		a.x * b,
		a.y * b,
		a.z * b,
		a.w * b);
}

bool operator==(const quat &left, const quat &right)
{
	return (fabsf(left.x - right.x) <= QUAT_EPSILON && fabsf(left.y - right.y) <= QUAT_EPSILON && fabsf(left.z - right.z) <= QUAT_EPSILON && fabsf(left.w - left.w) <= QUAT_EPSILON);
}

bool operator!=(const quat &a, const quat &b)
{
	return !(a == b);
}

bool sameOrientation(const quat &left, const quat &right)
{
	return (fabsf(left.x - right.x) <= QUAT_EPSILON && fabsf(left.y - right.y) <= QUAT_EPSILON && fabsf(left.z - right.z) <= QUAT_EPSILON && fabsf(left.w - left.w) <= QUAT_EPSILON) || (fabsf(left.x + right.x) <= QUAT_EPSILON && fabsf(left.y + right.y) <= QUAT_EPSILON && fabsf(left.z + right.z) <= QUAT_EPSILON && fabsf(left.w + left.w) <= QUAT_EPSILON);
}

float dot(const quat &a, const quat &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

quat operator*(const quat &Q1, const quat &Q2)
{
	return quat(
		Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w);
}

Vec3 operator*(const quat &q, const Vec3 &v)
{
	Vec3 vector = Vec3(q.x, q.y, q.z);
	return Vec3(q.x, q.y, q.z) * 2.0f * dot(vector, v) +
		   v * (q.w * q.w - dot(vector, vector)) +
		   cross(vector, v) * 2.0f * q.w;
}

quat mix(const quat &from, const quat &to, float t)
{
	return from * (1.0f - t) + to * t;
}

quat nlerp(const quat &from, const quat &to, float t)
{
	return (from + (to - from) * t).normalized();
}

quat operator^(const quat &q, float f)
{
	float angle = 2.0f * acosf(q.w);
	Vec3 axis = Vec3(q.x, q.y, q.z).Normalized();

	float halfCos = cosf(f * angle * 0.5f);
	float halfSin = sinf(f * angle * 0.5f);

	return quat(
		axis.x * halfSin,
		axis.y * halfSin,
		axis.z * halfSin,
		halfCos);
}

quat slerp(const quat &start, const quat &end, float t)
{
	if (fabsf(dot(start, end)) > 1.0f - QUAT_EPSILON)
		return nlerp(start, end, t);
	return (((start.inverse() * end) ^ t) * start).normalized();
}

quat lookRotation(const Vec3 &direcion, const Vec3 &up)
{
	// Find orthonormal basis vectors
	Vec3 f = direcion.Normalized();
	Vec3 u = up.Normalized();
	Vec3 r = cross(u, f);
	u = cross(f, r);

	// From world forward to object forward
	quat f2d = fromTo(Vec3(0, 0, 1), f);

	// what direction is the new object up?
	Vec3 objectUp = f2d * Vec3(0, 1, 0);
	// From object up to desired up
	quat u2u = fromTo(objectUp, u);

	// Rotate to forward direction first, then twist to correct up
	quat result = f2d * u2u;
	// Don’t forget to normalize the result
	return result.normalized();
}