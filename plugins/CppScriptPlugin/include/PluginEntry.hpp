/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.hpp header
*/

#ifndef LIBSFML_H_
#define LIBSFML_H_

#include <unordered_map>

#include "Core/Application.hpp"

#include "Plugin System (Not used)/api.hpp"
#include "Plugin/IPlugin.hpp"
#include "ScriptableBehaviour.hpp"
#include "NativeScript.hpp"

namespace fox
{
    class CppScriptPlugin : public IPlugin
    {
    public:
        CppScriptPlugin() = default;
        ~CppScriptPlugin() = default;

        void plug(Application& app) override;
        void unplug(Application &app) override;
        const std::string &get_name() const override;
        [[nodiscard]] int get_version() const override;

    private:
        Application* m_oApp = nullptr;
    };
}

#endif /* !LIBSFML_H_ */
