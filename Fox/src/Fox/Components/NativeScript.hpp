/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** NativeScript.hpp
*/

#ifndef ECS_NATIVESCRIPT_HPP
#define ECS_NATIVESCRIPT_HPP

#include <Reflection.hpp>
#include "Component.hpp"

class ScriptableBehaviour;

struct NativeScript
{
    ScriptableBehaviour* Instance = nullptr;

    ScriptableBehaviour* (*InstantiateScript)();
    void (*DestroyScript)(NativeScript*);

    template<typename T>
    void Bind()
    {
        InstantiateScript = []() { return static_cast<ScriptableBehaviour*>(new T()); };
        DestroyScript = [](NativeScript* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
    }
};
//
//using ScriptCreator = std::unique_ptr<ScriptableBehaviour> (*)();
//using StringHash = std::hash<std::string>;
//
//template <typename script_class>
//std::unique_ptr<ScriptableBehaviour> CreateScript()
//{
//    return std::make_unique<script_class>();
//}
//
//bool RegisterScript(size_t tag, ScriptCreator func);
//bool RegisterScriptName(const char* name);
//
//#define REGISTER_SCRIPT(type) \
//bool reg_##type { RegisterScript(StringHash()(#type), CreateScript<type>) }; \
//bool name_##type{ RegisterScriptName(#type) }; \
//
//#define REGISTER_CLASS_MEMBER(type, name)

#endif //ECS_NATIVESCRIPT_HPP
