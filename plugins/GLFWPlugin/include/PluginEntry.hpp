/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.hpp header
*/

#ifndef LIBSFML_H_
#define LIBSFML_H_

#include <unordered_map>

#include <Core/Application.hpp>

#include "api.hpp"
#include "Plugin/IPlugin.hpp"

namespace fox
{
    class GLFWPlugin : public WindowPlugin
    {
    public:
        GLFWPlugin() = default;
        ~GLFWPlugin() = default;

        void plug(Application& app) override;
        void unplug(Application &app) override;
        ref<Window> CreateWindow(const WindowProps& props = WindowProps()) override;

        const std::string &get_name() const override;
        [[nodiscard]] int get_version() const override;

    private:
        Application* m_oApp = nullptr;
    };
}

#endif /* !LIBSFML_H_ */