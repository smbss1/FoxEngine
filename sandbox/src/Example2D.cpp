//
// Created by samuel on 28/06/2021.
//

#include <Time.hpp>
#include <ImGui/imgui.h>
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

    fox::Renderer2D::ResetStats();

    fox::RendererCommand::SetClearColor({0, 0, 0, 1});
    fox::RendererCommand::Clear();

    static float rotation = 0.0f;
    rotation += Time::delta_time * 30.f;

    fox::Renderer2D::BeginScene(m_Camera.GetCamera());
    fox::Renderer2D::DrawQuad({-1, 0}, {0.8f, 0.8f}, {1.f, 0.f, 0.f, 1.f});
    fox::Renderer2D::DrawRotatedQuad({-2, 0}, {0.8f, 0.8f}, rotation,{1.f, 0.f, 0.f, 1.f});
    fox::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.f});
    fox::Renderer2D::DrawQuad({0.2f, 0.5f}, {0.5f, 0.5f}, m_pEpitechLogo);
    fox::Renderer2D::DrawRotatedQuad({0.0f, 0.0f}, {0.5f, 0.5f}, 45.0f, m_pEpitechLogo);
//    fox::Renderer2D::EndScene();
//
//    fox::Renderer2D::BeginScene(m_Camera.GetCamera());
    for (float y = -50.0f; y < 50.0f; y += 0.5f)
    {
        for (float x = -50.0f; x < 50.0f; x += 0.5f)
        {
            glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
            fox::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
        }
    }
    fox::Renderer2D::EndScene();
}

void Example2DState::OnImGui()
{
    ImGui::Begin("Settings");

    auto stats = fox::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();
}
