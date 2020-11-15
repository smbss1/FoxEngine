/*
** EPITECH PROJECT, 2019
** FoxEngine
** File description:
** FoxEngine
*/

#pragma once

#include "Core/World.hpp"
#include "TextureManager.hpp"
#include "ResourceManager.hpp"

namespace sf
{
    class RenderWindow;
} // namespace sf

class VM;

namespace Fox
{
    class Engine
    {
    public:
        World m_oWorld;
        ResourceManager m_oResourceManager;

        Engine();

        void Start(sf::RenderWindow& window);
        void Run();
        void Stop();
    };
} // namespace Fox
