//
// Created by samuel on 01/07/2021.
//

#ifndef FOXENGINE_IMGUISTATE_HPP
#define FOXENGINE_IMGUISTATE_HPP

#include "Core/State.hpp"

namespace fox
{
    class ImGuiState : public State
    {
    public:
        ImGuiState();

        ~ImGuiState() override = default;

        void OnEnter() override;

        void OnExit() override;

        void OnEvent(fox::Event &event) override;

        void OnImGui() override;

        void Begin();
        void End();

        void BlockEvents(bool block) { m_bBlockEvents = block; }

    private:
        void SetDarkThemeColors();

        bool m_bBlockEvents = false;
    };
}

#endif //FOXENGINE_IMGUISTATE_HPP
