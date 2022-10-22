// //
// // Created by samuel on 28/06/2021.
// //

// #ifndef FOXENGINE_ANIMATOR_STATE_HPP
// #define FOXENGINE_ANIMATOR_STATE_HPP

// #include "FoxEngine.hpp"
// #include <Components/Animator/AnimationPlayer.hpp>

// class AnimationPlayerState : public fox::State
// {
// public:
//     AnimationPlayerState()
//             : m_Camera(1280.f / 720.f, true)
//             , fox::State("Animator") { }

//     ~AnimationPlayerState() override = default;
//     void OnEnter() override;
//     void OnExit() override;
//     void OnEvent(fox::Event& event) override;
//     void OnUpdate() override;
//     void OnImGui() override;

// private:
//     fox::OrthographicCameraController m_Camera;

//     fox::Ref<fox::Scene> m_pActiveScene;

//     fox::AnimationPlayer* m_AnimationPlayer{};
// };


// #endif //FOXENGINE_ANIMATOR_STATE_HPP
