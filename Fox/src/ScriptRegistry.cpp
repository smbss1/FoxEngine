//
// Created by samuel on 03/07/2021.
//

#include "Components/NativeScript.hpp"
#include "FoxEngine.hpp"

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