/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include <Core/Managers/PluginManager.hpp>
#include <Core/Window.hpp>
#include <GlfwWindow.hpp>
#include "PluginEntry.hpp"

namespace fox
{
    void GLFWPlugin::plug(Application &app)
    {
        m_oApp = &app;
        std::cout << "[GLFWPlugin] Init!" << std::endl;
    }

    void GLFWPlugin::unplug(Application &app)
    {
    }

    ref<Window> GLFWPlugin::CreateWindow(const WindowProps& props)
    {
        return fox::new_ref<GlfwWindow>(props);
    }

    const std::string &GLFWPlugin::get_name() const
    {
        return "GLFW";
    }

    int GLFWPlugin::get_version() const
    {
        return 1;
    }
}

extern "C" void RegisterPlugin(fox::PluginManager &pm)
{
    fox::ref<fox::GLFWPlugin> plugin = fox::new_ref<fox::GLFWPlugin>();
    pm.SetWindowPlugin(plugin);
    pm.AddPlugin(plugin);
}