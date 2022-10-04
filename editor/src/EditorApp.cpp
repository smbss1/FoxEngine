
#include <FoxEngine.hpp>
#include <Core/EntryPoint.hpp>

#include <Core/Input/InputManager.hpp>
#include <Core/State.hpp>
#include <Core/Managers/StateMachine.hpp>
#include <Renderer/EditorCamera.hpp>
#include <EditorState.hpp>
#include <Time.hpp>
#include <Core/Input/Input.hpp>

class EditorApp : public fox::Application
{
    public:
    EditorApp(int argc, char** argv);
        ~EditorApp() override;
        void init() override;

    protected:
    private:
};

// #include "ScriptableBehaviour.hpp"

namespace fox
{
    // class Test : public ScriptableBehaviour
    // {
    //     float value;
    // public:
    //     ~Test() override = default;

    // protected:
    //     void on_create() override
    //     {
    //         auto& translation = get_component<TransformComponent>()->position;
    //         translation.x = rand() % 10 - 5.0f;
    //     }

    //     void on_update() override
    //     {
    //         auto& translation = get_component<TransformComponent>()->position;

    //         float speed = 5.0f;

    //         if (Input::IsKeyPressed(KeyCode::A))
    //             translation.x -= speed * Time::delta_time;
    //         if (Input::IsKeyPressed(KeyCode::D))
    //             translation.x += speed * Time::delta_time;
    //         if (Input::IsKeyPressed(KeyCode::W))
    //             translation.y += speed * Time::delta_time;
    //         if (Input::IsKeyPressed(KeyCode::S))
    //             translation.y -= speed * Time::delta_time;
    //     }

    //     void on_destroy() override { }
    //     int i = 0;
    // };
}

EditorApp::EditorApp(int argc, char** argv) : fox::Application(argc, argv)
{
    fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
    sceneManager.PushState(new fox::EditorState());
}

EditorApp::~EditorApp() { }

void EditorApp::init()
{
    // fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
    // sceneManager.PushState(new fox::EditorState());
}

fox::Application* fox::CreateApp(int argc, char** argv)
{
    return new EditorApp(argc, argv);
}
