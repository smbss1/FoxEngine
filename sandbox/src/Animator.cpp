// //
// // Created by samuel on 28/06/2021.
// //

// #include <Time.hpp>
// #include <imgui.h>
// #include <Core/Input/Input.hpp>
// #include "AnimationPlayerState.hpp"

// void AnimationPlayerState::OnEnter()
// {
//     m_pActiveScene = fox::new_ref<fox::Scene>(GetApp());

//     auto e = m_pActiveScene->NewEntity("");
//     auto& oAnimPlayer = e.add<fox::AnimationPlayer>();
//     auto& idle = oAnimPlayer.AddAnimation("Idle");
//     auto& run = oAnimPlayer.AddAnimation("Run");

//     m_AnimationPlayer = &oAnimPlayer;

//     fox::info("%", rttr::type::get<fox::AnimationPlayer>().get_name());


// //    oAnimPlayer.current.add_update_event([](property<Timeline*>* pAnim)
// //       {
// //           fox::info("Current Animation: %", (*pAnim)->Name);
// //       });
// }

// void AnimationPlayerState::OnExit()
// {}

// void AnimationPlayerState::OnEvent(fox::Event& event)
// {
//     m_Camera.OnEvent(event);
// }

// void AnimationPlayerState::OnUpdate()
// {
//     m_Camera.OnUpdate();
//     m_pActiveScene->OnUpdateRuntime();

//     m_AnimationPlayer->run();

// //    if (m_AnimationPlayer->Current.get())
// //        fox::info("Current Animation: %", m_AnimationPlayer->Current.get()->Name);

//     if (fox::Input::IsKeyPressed(fox::KeyCode::G))
//         m_AnimationPlayer->play("Idle");

//     if (fox::Input::IsKeyPressed(fox::KeyCode::H))
//         m_AnimationPlayer->play("Run");

//     if (fox::Input::IsKeyPressed(fox::KeyCode::J))
//         m_AnimationPlayer->stop();

//     fox::Renderer2D::ResetStats();

//     fox::RendererCommand::SetClearColor({0, 0, 0, 1});
//     fox::RendererCommand::Clear();
// }

// void AnimationPlayerState::OnImGui()
// {
//     ImGui::Begin("Settings");

//     auto stats = fox::Renderer2D::GetStats();
//     ImGui::Text("Renderer2D Stats:");
//     ImGui::Text("Delta time: %f", Time::delta_time);
//     ImGui::Text("Draw Calls: %d", stats.DrawCalls);
//     ImGui::Text("Quads: %d", stats.QuadCount);
//     ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
//     ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

//     ImGui::End();
// }
