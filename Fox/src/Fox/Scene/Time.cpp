/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Time.cpp
*/

#include "Utils/PlatformUtils.hpp"

namespace fox
{
    float Time::time = 0;
    float Time::delta_time = 0;
    float Time::fixed_delta_time = 0;
    float Time::factor_physics = .5f;
}
