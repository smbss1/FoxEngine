/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Track.hpp
*/

#ifndef _H_TRACK_
#define _H_TRACK_

#include <vector>
#include <cstring>
#include <iostream>
#include "Frame.hpp"
#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"
#include "Easing.hpp"

namespace TrackHelpers
{
    /**
     * @brief Use for linear interpolation
     * @tparam T the type of the interpolated value
     * @param a from value
     * @param b to value
     * @param t delta time
     * @return interpolated value
     */
	template<typename T>
	inline T Interpolate(T a, T b, float t) {
		return a + (b - a) * t;
	}

    /**
     * @brief Use for linear interpolation
     * @param a from value
     * @param b to value
     * @param t delta time
     * @return interpolated value
     */
	template<>
	inline fox::Vec3 Interpolate(fox::Vec3 a, fox::Vec3 b, float t) {
		return lerp(a, b, t);
	}

    /**
     * @brief Use for linear interpolation
     * @param a from value
     * @param b to value
     * @param t delta time
     * @return interpolated value
     */
	template<>
	inline fox::quat Interpolate(fox::quat a, fox::quat b, float t) {
        fox::quat result = mix(a, b, t);
		if (dot(a, b) < 0) { // Neighborhood
			result = mix(a, -b, t);
		}
		return result.normalized(); //NLerp, not slerp
	}


	/**
	 * @brief Hermite helpers
	 */
	inline float AdjustHermiteResult(float f) {
		return f;
	}

    /**
	 * @brief Hermite helpers
	 */
	inline fox::Vec3 AdjustHermiteResult(const fox::Vec3& v) {
		return v;
	}

    /**
	 * @brief Hermite helpers
	 */
	inline fox::quat AdjustHermiteResult(const fox::quat& q) {
		return q.normalized();
	}

    /**
     * @brief Get the neighborhood of this 2 value
     * @tparam T the type of the value to compare
     * @param a the first point to compare
     * @param b the second point to compare
     */
	template<typename T>
	inline void Neighborhood(T a, T b) { }

    /**
     * @brief Get the neighborhood of this 2 value
     * @param a the first point to compare
     * @param b the second point to compare
     */
	template<>
	inline void Neighborhood(fox::quat a, fox::quat b) {
		if (dot(a, b) < 0) {
			b = -b;
		}
	}
}; // End Track Helpers namespace

class ITrack
{
public:

    virtual ~ITrack() {}
    /**
     * @brief Run the track
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     */
	virtual void Sample(float time, bool looping) = 0;

    /**
     * @brief Return the number of keyframe
     * @return the number of keyframe
     */
	virtual std::size_t size() = 0;

    /**
     * @brief Return the start time of the track
     * @return the start time of the track
     */
	virtual float GetStartTime() = 0;

    /**
     * @brief Return the end time of the track
     * @return the end time of the track
     */
	virtual float GetEndTime() = 0;

    /**
     * @brief Check if the track have keyframes otherwise false
     * @return true if the track have keyframes otherwise false
     */
	virtual bool IsValid() = 0;
};

template<typename T>
class Track : public ITrack
{
protected:
	std::vector<Frame<T>> m_vFrames;
	Easing m_eEasing;
	T* m_pTarget;

    std::function<void(T)> settter;

protected:
    /**
     * @brief Run the track with constant interpolation
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     * @return interpolated value
     */
	T SampleConstant(float time, bool looping)
	{
		int frame = FrameIndex(time, looping);
		if (frame < 0 || frame >= (int)m_vFrames.size()) {
			return T();
		}

		return m_vFrames[frame].m_Value;
	}

    /**
     * @brief Run the track with linear interpolation
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     * @return interpolated value
     */
	T SampleLinear(float time, bool looping)
	{
		int thisFrame = FrameIndex(time, looping);

		if (thisFrame < 0 || thisFrame >= (int)(m_vFrames.size() - 1)) {
			return T();
		}
		int nextFrame = thisFrame + 1;

		float trackTime = AdjustTimeToFitTrack(time, looping);
		float frameDelta = m_vFrames[nextFrame].m_fTime - m_vFrames[thisFrame].m_fTime;
		if (frameDelta <= 0.0f) {
			return T();
		}
		float t = (trackTime - m_vFrames[thisFrame].m_fTime) / frameDelta;

		T start = m_vFrames[thisFrame].m_Value;
		T end = m_vFrames[nextFrame].m_Value;

		return TrackHelpers::Interpolate(start, end, t);
	}

    /**
     * @brief Run the track with cubic interpolation
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     * @return interpolated value
     */
	T SampleCubic(float time, bool looping)
	{
		int thisFrame = FrameIndex(time, looping);
		if (thisFrame < 0 || thisFrame >= (int)(m_vFrames.size() - 1)) {
			return T();
		}
		int nextFrame = thisFrame + 1;

		float trackTime = AdjustTimeToFitTrack(time, looping);
		float frameDelta = m_vFrames[nextFrame].m_fTime - m_vFrames[thisFrame].m_fTime;
		if (frameDelta <= 0.0f) {
			return T();
		}
		float t = (trackTime - m_vFrames[thisFrame].m_fTime) / frameDelta;

		T point1 = m_vFrames[thisFrame].m_Value;
		T slope1 = m_vFrames[thisFrame].m_Out;
		slope1 = slope1 * frameDelta;

		T point2 = m_vFrames[nextFrame].m_Value;
		T slope2 = m_vFrames[nextFrame].m_In;
		slope2 = slope2 * frameDelta;

		return Hermite(t, point1, slope1, point2, slope2);
	}

	T Hermite(float time, const T& point1, const T& slope1,	const T& point2, const T& slope2)
	{
		float tt = time * time;
		float ttt = tt * time;

		T p2 = point2;
		TrackHelpers::Neighborhood(point1, p2);

		float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
		float h2 = -2.0f * ttt + 3.0f * tt;
		float h3 = ttt - 2.0f * tt + time;
		float h4 = ttt - tt;

		T result = point1 * h1 + p2 * h2 + slope1 * h3 + slope2 * h4;
		return TrackHelpers::AdjustHermiteResult(result);
	}

    /**
     * @brief Get the index of the frame using the time
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     * @return the index of the frame using the time
     */
	int FrameIndex(float time, bool looping)
	{
		auto size = (unsigned int)m_vFrames.size();
		if (size <= 1)
			return -1;
		if (looping)
		{
			float startTime = m_vFrames[0].m_fTime;
			float endTime = m_vFrames[size - 1].m_fTime;
			float duration = endTime - startTime;

			time = fmodf(time - startTime, endTime - startTime);
			if (time < 0.0f)
				time += endTime - startTime;
			time = time + startTime;
		}
		else
		{
			if (time <= m_vFrames[0].m_fTime)
				return 0;
			if (time >= m_vFrames[size - 2].m_fTime)
				return (int)size - 2;
		}
		for (int i = (int)size - 1; i >= 0; --i)
			if (time >= m_vFrames[i].m_fTime)
				return i;
		// Invalid code, we should not reach here!
		return -1;
	} // End of FrameIndex

    /**
     * @brief Adjust the time to fit with the track
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     * @return the adjusted time
     */
	float AdjustTimeToFitTrack(float time, bool looping)
	{
		auto size = (unsigned int)m_vFrames.size();
		if (size <= 1)
			return 0.0f;

		float startTime = m_vFrames[0].m_fTime;
		float endTime = m_vFrames[size - 1].m_fTime;
		float duration = endTime - startTime;
		if (duration <= 0.0f)
			return 0.0f;
		if (looping) {
			time = fmodf(time - startTime, endTime - startTime);
			if (time < 0.0f)
				time += endTime - startTime;
			time = time + startTime;
		} else {
			if (time <= m_vFrames[0].m_fTime)
				time = startTime;
			if (time >= m_vFrames[size - 1].m_fTime)
				time = endTime;
		}
		return time;
	}

public:
    /**
     * @brief COnstructor
     */
	Track() : m_pTarget(nullptr), settter(nullptr), m_vFrames()
	{
		m_eEasing = Easing::Linear;
	}

    ~Track() override = default;

    /**
     * @brief Add a new keyframe, at the time giving the paramater, in the track
     * @param time the time where the keyframe will be added
     * @param value the value of the keyframe
     * @return the class itself
     */
    Track<T>& then(float time, T value)
    {
        return then(time, T(), T(), value);
    }

    /**
     * @brief Add a new keyframe, at the time giving the paramater, in the track
     * @param time the time where the keyframe will be added
     * @param in smooth interpolation
     * @param out smooth interpolation
     * @param value the value of the keyframe
     * @return the class itself
     */
	Track<T>& then(float time, T in, T out, T value)
    {
        Frame<T> result;
        result.m_fTime = time;
        result.m_In = in;
        result.m_Value = value;
        result.m_Out = out;
		m_vFrames.push_back(std::move(result));
        return *this;
    }

    /**
     * @brief Check if the track have keyframes otherwise false
     * @return true if the track have keyframes otherwise false
     */
	bool IsValid() override
	{
		return m_vFrames.size() > 0;
	}

    /**
     * @brief Return the number of keyframe
     * @return the number of keyframe
     */
	std::size_t size() override
	{
		return m_vFrames.size();
	}

    /**
     * @brief Set the type of interpolation to use
     * @param e the type of interpolation
     */
	void ease(Easing e)
	{
		m_eEasing = e;
	}

    /**
     * @brief Set the reference of the value to be modified during the simulation
     * @param target the reference
     * @return the class itself
     */
	Track<T>& apply(T& target)
	{
		m_pTarget = &target;
        return *this;
	}

    /**
      * @brief Set the setter of the value to be modified during the simulation
     * @tparam TVar the type of the class
     * @param obj the class for the setter
     * @param set the setter
     * @return the class itself
     */
	template<typename TVar>
    Track<T>& apply(TVar* obj, void (TVar::*set)(T))
	{
		settter = [&obj, set](T val) -> void { std::cerr << val << std::endl; (obj->*set)(val); };
		return *this;
	}

    /**
     * @brief Return the start time of the track
     * @return the start time of the track
     */
	float GetStartTime() override
	{
		return m_vFrames[0].m_fTime;
	}

    /**
     * @brief Return the end time of the track
     * @return the end time of the track
     */
	float GetEndTime() override
	{
		return m_vFrames[m_vFrames.size() - 1].m_fTime;
	}

    /**
     * @brief Run the track
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     */
	void Sample(float time, bool looping) override
	{
		if (m_pTarget)
		{
			SampleTarget(time, looping);
		} else {
			SampleSetter(time, looping);
		}
	}

    /**
     * @brief Run the track using setter function
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     */
	void SampleSetter(float time, bool looping)
	{
		if (!settter)
		{
			std::cerr << "Setter not set in Track" << std::endl;
			return;
		}
		switch (m_eEasing)
		{
			case Easing::Constant:
				settter(SampleConstant(time, looping));
				break;
			case Easing::Linear:
				settter(SampleLinear(time, looping));
				break;
			case Easing::Cubic:
				settter(SampleCubic(time, looping));
				break;
			default:
				break;
		}
	}

    /**
     * @brief Run the track using class member
     * @param time current time of the animation
     * @param looping set it to true and the track will loop
     */
	void SampleTarget(float time, bool looping)
	{
		if (!m_pTarget)
		{
			std::cerr << "Target not set in Track" << std::endl;
			return;
		}
		switch (m_eEasing)
		{
			case Easing::Constant:
				*m_pTarget = SampleConstant(time, looping);
				break;
			case Easing::Linear:
				*m_pTarget = SampleLinear(time, looping);
				break;
			case Easing::Cubic:
				*m_pTarget = SampleCubic(time, looping);
				break;
			default:
				break;
		}
	}

    /**
     * @brief Get the keyframe at the index given in parameter (no out of range check)
     * @param index the position of the keyframe in the list
     * @return the keyframe
     */
	Frame<T>& operator[](unsigned int index)
	{
		return m_vFrames[index];
	}
};

#endif