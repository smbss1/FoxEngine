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
#include "Locator.hpp"
#include "SimpleSignal.hpp"

namespace sf
{
    class RenderWindow;
} // namespace sf

namespace Fox
{
    extern Locator oLocator;
    class Engine
    {
    public:
        World m_oWorld;
        // ResourceManager m_oResourceManager;
        Simple::Signal<void (float)> m_oOnUpdate;
        Simple::Signal<void ()> m_oOnSceneLoaded;

        std::string m_strProjectDir;
        std::string m_strAssetDir;

        Engine(const std::string& strProjectDir);

        void Start(sf::RenderWindow& window);
        void Run();
        void Stop();
    };
} // namespace Fox
