/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Frame.hpp
*/

#ifndef _FOX_FRAME_HPP_
#define _FOX_FRAME_HPP_

#include "Core/EventNotifier.hpp"

/***
 * @brief This struct store the key frame of the animation
 * @tparam T the type of the key value to store
 */
template<typename T>
struct Frame
{
public:
    Frame() {}
    ~Frame() = default;

	T m_Value;
	T m_In;
	T m_Out;
	float m_fTime;
};

#endif