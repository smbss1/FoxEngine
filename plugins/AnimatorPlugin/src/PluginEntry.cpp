/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include <Plugin/PluginManager.hpp>
#include "PluginEntry.hpp"

namespace fox
{
    void AnimatorPlugin::plug(Application &app)
    {
        m_oApp = &app;
        std::cout << "[AnimatorPlugin] Init!" << std::endl;

//        app.get_world().system<Animator>()
//            .each([](Entity& e, Animator& anim)
//            {
//                anim.run();
//            });
    }

    void AnimatorPlugin::unplug(Application &app)
    {
    }

    const std::string &AnimatorPlugin::get_name() const
    {
        return "AnimatorPlugin";
    }

    int AnimatorPlugin::get_version() const
    {
        return 1;
    }
}

extern "C" void RegisterPlugin(fox::PluginManager &pm)
{
//    fox::ref<fox::AnimatorPlugin> plugin = fox::new_ref<fox::AnimatorPlugin>();
//    pm.AddPlugin(plugin);
}