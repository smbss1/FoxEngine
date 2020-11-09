/*
** EPITECH PROJECT, 2019
** FoxEngine
** File description:
** FoxEngine
*/

#pragma once

class VM;

#include "Core/World.hpp"

namespace Fox
{
    class Engine
    {
    public:
        World m_oWorld;

        Engine();

        void Start();
        void Run();
        void Stop();
    };
} // namespace Fox
