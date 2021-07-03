/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** EntryPoint.cpp
*/

#include <Core/Logger/Logger.hpp>
#include "Components/NativeScript.hpp"
#include "FoxEngine.hpp"

extern fox::Application* CreateApp(int argc, char** argv);

int main(int argc, char**argv)
{
    fox::Application *pApp = nullptr;
    try {
        pApp = CreateApp(argc, argv);
        pApp->run();
    } catch (std::exception& e) {
        fox::error("%", e.what());
    }
    fox::info("Application stopped");
    if (pApp)
        delete pApp;
    return 0;
}

std::unordered_map<size_t, ScriptCreator>& Registry()
{
    static std::unordered_map<size_t, ScriptCreator> registry;
    return registry;
}

std::unordered_map<size_t, std::string>& NameRegistry()
{
    static std::unordered_map<size_t, std::string>registry;
    return registry;
}

bool RegisterScript(size_t tag, ScriptCreator func)
{
    bool res = Registry().insert({tag, func}).second;
    FOX_CORE_ASSERT(res, "Script registry insertion Failed");
    return res;
}

bool RegisterScriptName(const char* name)
{
    NameRegistry().insert({StringHash()(name) , name});
    return true;
}

extern "C"
std::unordered_map<size_t, std::string> GetScriptsNames()
{
    return NameRegistry();
}

extern "C"
std::unordered_map<size_t, ScriptCreator> GetScripts()
{
    return Registry();
}