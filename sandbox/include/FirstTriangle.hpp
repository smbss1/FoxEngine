//
// Created by samuel on 28/06/2021.
//

#ifndef FOXENGINE_FIRSTTRIANGLE_HPP
#define FOXENGINE_FIRSTTRIANGLE_HPP

#include "FoxEngine.hpp"

class FirstTriangleState : public fox::State
{
public:
    FirstTriangleState()
            : m_Camera(1280.f / 720.f, true)
            , fox::State("FirstTriangle") { }

    ~FirstTriangleState() override = default;
    void OnEnter() override;
    void OnExit() override;
    void OnEvent(fox::Event& event) override;
    void OnUpdate() override;

private:
    fox::OrthographicCameraController m_Camera;

    fox::ref<fox::VertexArray> va;
    fox::ref<fox::Shader> shader;
};


#endif //FOXENGINE_FIRSTTRIANGLE_HPP
