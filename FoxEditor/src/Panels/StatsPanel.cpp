//
// Created by samuel on 09/10/22.
//

#include "StatsPanel.hpp"

#include "imgui.h"
#include "Renderer/Renderer2D.hpp"

void fox::StatsPanel::OnImGui()
{
    ImGuiIO &io = ImGui::GetIO();

    ImGui::Begin("Stats");
    {
        auto stats = fox::Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    }
    ImGui::End();
}
