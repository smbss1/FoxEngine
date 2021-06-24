

#include <Core/Input/InputManager.hpp>
#include <Core/Logger/Logger.hpp>
#include "EditorApp.hpp"
#include "ScriptableBehaviour.hpp"
#include "NativeScript.hpp"
#include "Animator.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"

class EditorLayer : public fox::Scene
{
public:
    EditorLayer() { }
    EditorLayer(const std::string& name) : fox::Scene(name) { }
    ~EditorLayer() { }

    virtual void on_create(fox::Application& app) override { }

    virtual void on_enable(fox::Application& app) override
    {
    }

    void on_destroy(fox::Application& app) override { }
    void on_update(fox::Application& app) override
    {
    }
};

SandboxApp::SandboxApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

SandboxApp::~SandboxApp() { }

void SandboxApp::init()
{
    fox::SceneManager& sceneManager = get<fox::SceneManager>().value();
    sceneManager.add<EditorLayer>("Example");
    sceneManager.switch_to("Example");
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new SandboxApp(argc, argv);
}