//
// Created by samuel on 28/06/2021.
//

#include <Time.hpp>
#include <imgui.h>
#include <Core/Input/Input.hpp>
#include "AnimatorState.hpp"

void AnimatorState::OnEnter()
{
    m_pActiveScene = fox::new_ref<fox::Scene>(GetApp());

    auto e = m_pActiveScene->NewEntity("");
    auto& animator = e.add<Animator>();
    auto& idle = animator.add_anim("Idle");
    auto& run = animator.add_anim("Run");
    animator.AddParam("doRun", AnimVarType::TRIGGER);
    animator.AddTransition(fox::new_ref<TriggerCondition>(*animator.GetParam("doRun")), idle.ID, run.ID);
    animator.AddTransition(fox::new_ref<Condition>(), run.ID, idle.ID);

    anim = &animator;

    anim->current.add_update_event([](property<Timeline*>* pAnim)
       {
           fox::info("Current Animation: %", (*pAnim)->Name);
       });
}

void AnimatorState::OnExit()
{}

void AnimatorState::OnEvent(fox::Event& event)
{
    m_Camera.OnEvent(event);
}

void AnimatorState::OnUpdate()
{
    m_Camera.OnUpdate();
    m_pActiveScene->OnUpdateRuntime();

    anim->run();

    if (fox::Input::IsKeyPressed(fox::Key::G))
    {
        anim->SetTrigger("doRun");
    }

    fox::Renderer2D::ResetStats();

    fox::RendererCommand::SetClearColor({0, 0, 0, 1});
    fox::RendererCommand::Clear();
}

void AnimatorState::OnImGui()
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
