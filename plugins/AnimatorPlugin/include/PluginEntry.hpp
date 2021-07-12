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
#include "Components/Animator/AnimationPlayer.hpp"

namespace fox
{
    class AnimatorPlugin : public IPlugin
    {
    public:
        AnimatorPlugin() = default;
        ~AnimatorPlugin() = default;

        void plug(Application& app) override;
        void unplug(Application &app) override;
        const std::string &get_name() const override;
        [[nodiscard]] int get_version() const override;

    private:
        Application* m_oApp = nullptr;
    };
}

#endif /* !LIBSFML_H_ */