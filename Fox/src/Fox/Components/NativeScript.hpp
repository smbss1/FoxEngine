/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** NativeScript.hpp
*/

#ifndef ECS_NATIVESCRIPT_HPP
#define ECS_NATIVESCRIPT_HPP

namespace fox
{
    class ScriptableBehaviour;

    struct NativeScript
    {
        ScriptableBehaviour *Instance = nullptr;

        ScriptableBehaviour *(*InstantiateScript)();

        void (*DestroyScript)(NativeScript *);

        template<typename T>
        void Bind()
        {
            InstantiateScript = []() { return static_cast<ScriptableBehaviour *>(new T()); };
            DestroyScript = [](NativeScript *nsc)
            {
                delete nsc->Instance;
                nsc->Instance = nullptr;
            };
        }
    };

//    struct PrefabComponent
//    {
//        UUID ID;
//
//        PrefabComponent() = default;
//        PrefabComponent(const PrefabComponent&) = default;
//    };
}

#endif //ECS_NATIVESCRIPT_HPP
