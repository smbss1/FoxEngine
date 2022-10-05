//
// Created by samuel on 28/06/2021.
//

#include "Save for later/Time.hpp"
#include <imgui.h>
#include "SpriteSheet.hpp"

void SpriteSheetLayer::OnAttach()
{
    m_pSpriteSheet = fox::Texture2D::Create("sandbox/assets/RPGpack_sheet_2X.png");
    m_TextureStairs = fox::SubTexture2D::CreateFromCoords(m_pSpriteSheet, {7,6}, {128, 128});
    m_TextureBarrel = fox::SubTexture2D::CreateFromCoords(m_pSpriteSheet, {8,2}, {128, 128});
    m_TextureTree = fox::SubTexture2D::CreateFromCoords(m_pSpriteSheet, {2,1}, {128, 128}, {1, 2});
}

void SpriteSheetLayer::OnDetach()
{}

void SpriteSheetLayer::OnEvent(fox::Event& event)
{
    m_Camera.OnEvent(event);
}

void SpriteSheetLayer::OnUpdate(fox::Timestep ts)
{
    m_Camera.OnUpdate();

    fox::Renderer2D::ResetStats();

    fox::RendererCommand::SetClearColor({0, 0, 0, 1});
    fox::RendererCommand::Clear();

    fox::Renderer2D::BeginScene(m_Camera.GetCamera());
    fox::Renderer2D::DrawQuad({0, 0}, {1.0f, 1.0f}, m_TextureStairs);
    fox::Renderer2D::DrawQuad({1, 0}, {1.0f, 1.0f}, m_TextureBarrel);
    fox::Renderer2D::DrawQuad({2, 0}, {1.0f, 2.0f}, m_TextureTree);
    fox::Renderer2D::EndScene();
}

void SpriteSheetLayer::OnImGuiRender()
{
    ImGui::Begin("Settings");

    auto stats = fox::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Delta time: %f", Time::delta_time);
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();
}
