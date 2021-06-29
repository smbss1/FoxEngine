//
// Created by samuel on 28/06/2021.
//

#ifndef FOXENGINE_FIRSTTRIANGLE_HPP
#define FOXENGINE_FIRSTTRIANGLE_HPP

#include "FoxEngine.hpp"

class Example2DState : public fox::State
{
public:
    Example2DState()
            : m_Camera(1280.f / 720.f, true)
            , fox::State("Example2D") { }

    ~Example2DState() override = default;
    void OnEnter() override;
    void OnExit() override;
    void OnEvent(fox::Event& event) override;
    void OnUpdate() override;

private:
    fox::OrthographicCameraController m_Camera;

    fox::ref<fox::Texture2D> m_pEpitechLogo;
};


#endif //FOXENGINE_FIRSTTRIANGLE_HPP