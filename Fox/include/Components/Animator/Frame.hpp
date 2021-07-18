/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Frame.hpp
*/

#ifndef _FOX_FRAME_HPP_
#define _FOX_FRAME_HPP_

#include "Reflection.hpp"

/***
 * @brief This struct store the key frame of the animation
 * @tparam T the type of the key value to store
 */
template<typename T0>
struct Frame
{
    REFLECTABLEV(Frame<T0>)

public:
    Frame() {}
    ~Frame() = default;

    T0 m_Value{};
    T0 m_In{};
    T0 m_Out{};
	float m_fTime{};
};

#endif