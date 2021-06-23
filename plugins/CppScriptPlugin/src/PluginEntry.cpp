/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include <Core/Managers/PluginManager.hpp>
#include "PluginEntry.hpp"

namespace fox
{
    void CppScriptPlugin::plug(Application &app)
    {
        m_oApp = &app;
        std::cout << "[CppScriptPlugin] Init!" << std::endl;

        app.get_world().system<NativeScript>().kind(ecs::OnUpdate)
            .each([&](Entity e, NativeScript& script)
                  {
                      script.on_update();
                  });

        app.get_world().system<NativeScript>().kind(ecs::OnAdd)
            .each([&](Entity e, NativeScript& script)
                  {
                      script.m_pWorld = e.get_world();
                      script.on_create_all(e.get_id(), app, *app.get<SceneManager>()->get_active());
                  });

        app.get_world().system<NativeScript>().kind(ecs::OnAddScript)
            .each([&](Entity e, NativeScript& script)
              {
                  script.on_create(e.get_id(), app, *app.get<SceneManager>()->get_active());
              });
    }

    const std::string &CppScriptPlugin::get_name() const
    {
        return "CppScript";
    }

    int CppScriptPlugin::get_version() const
    {
        return 1;
    }
}

extern "C" void RegisterPlugin(fox::PluginManager &pm)
{
    fox::ref<fox::CppScriptPlugin> plugin = fox::new_ref<fox::CppScriptPlugin>();
    pm.AddPlugin(plugin);
}