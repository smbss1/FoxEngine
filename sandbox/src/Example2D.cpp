//
// Created by samuel on 28/06/2021.
//

#include "Example2D.hpp"

void Example2DState::OnEnter()
{
    m_pEpitechLogo = fox::Texture2D::Create("sandbox/assets/epitech.png");
}

void Example2DState::OnExit()
{}

void Example2DState::OnEvent(fox::Event& event)
{
    m_Camera.OnEvent(event);
}

void Example2DState::OnUpdate()
{
    m_Camera.OnUpdate();

    fox::RendererCommand::SetClearColor({0, 0, 0, 1});
    fox::RendererCommand::Clear();

    fox::Renderer2D::BeginScene(m_Camera.GetCamera());
    fox::Renderer2D::DrawQuad({-1, 0}, {0.8f, 0.8f}, {1.f, 0.f, 0.f, 1.f});
    fox::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.f});
    fox::Renderer2D::DrawQuad({0.2f, 0.5f}, {0.5f, 0.5f}, m_pEpitechLogo);
    fox::Renderer2D::EndScene();
}