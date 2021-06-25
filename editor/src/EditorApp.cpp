

#include <Core/Input/InputManager.hpp>
#include <Core/Logger/Logger.hpp>
#include <Core/State.hpp>
#include <Core/Managers/StateMachine.hpp>
#include "SandboxApp.hpp"
#include "Animator.hpp"
#include "ImGui/imgui.h"

class EditorLayer : public fox::State
{
public:
    EditorLayer() : fox::State("Editor") { }
    ~EditorLayer() override = default;

    void OnEnter() override
    {}

    void OnExit() override
    {}

    void OnUpdate() override
    {}

    void OnImGui() override
    {
        ImGui::ShowDemoWindow();
    }
};

SandboxApp::SandboxApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

SandboxApp::~SandboxApp() { }

void SandboxApp::init()
{
    fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
    sceneManager.PushState(new EditorLayer);
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new SandboxApp(argc, argv);
}