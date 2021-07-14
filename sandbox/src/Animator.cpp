//
// Created by samuel on 28/06/2021.
//

#include <Utils/Time.hpp>
#include <imgui.h>
#include <Input/Input.hpp>
#include "AnimationPlayerState.hpp"

void AnimationPlayerState::OnEnter()
{
    m_pActiveScene = fox::new_ref<fox::Scene>(GetApp());

    auto e = m_pActiveScene->NewEntity("");
    auto& oAnimPlayer = e.add<fox::AnimationPlayer>();
    auto& idle = oAnimPlayer.AddAnimation("Idle");
    auto& run = oAnimPlayer.AddAnimation("Run");

    m_AnimationPlayer = &oAnimPlayer;

//    oAnimPlayer.Current +=
    fox::event::Delegate<void(int)> d1;
    d1 += [](int value) -> void
   {
        fox::info("Int: %", value);
   };

    d1(14);

    oAnimPlayer.Current += [](fox::properties::rw_property<Timeline*>* oAnim)
    {
        fox::info("Current Animation: %", oAnim->get()->Name);
    };
}

void AnimationPlayerState::OnExit()
{}

void AnimationPlayerState::OnEvent(fox::Event& event)
{
    m_Camera.OnEvent(event);
}

void AnimationPlayerState::OnUpdate()
{
    m_Camera.OnUpdate();
    m_pActiveScene->OnUpdateRuntime();

    m_AnimationPlayer->run();

    if (fox::Input::IsKeyPressed(fox::Key::G))
        m_AnimationPlayer->play("Idle");

    if (fox::Input::IsKeyPressed(fox::Key::H))
        m_AnimationPlayer->play("Run");

    if (fox::Input::IsKeyPressed(fox::Key::J))
        m_AnimationPlayer->stop();

    fox::Renderer2D::ResetStats();

    fox::RendererCommand::SetClearColor({0, 0, 0, 1});
    fox::RendererCommand::Clear();
}

void AnimationPlayerState::OnImGui()
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
