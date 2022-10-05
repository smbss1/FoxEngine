//
// Created by samuel on 01/07/2021.
//

#ifndef FOXENGINE_IMGUISTATE_HPP
#define FOXENGINE_IMGUISTATE_HPP

#include "Core/Layer.hpp"

namespace fox
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(Event& event) override;

        void Begin();
        void End();

        void BlockEvents(bool block) { m_bBlockEvents = block; }

    private:
        void SetDarkThemeColors();

        bool m_bBlockEvents = false;
    };
}

#endif //FOXENGINE_IMGUISTATE_HPP
