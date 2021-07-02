

#include <Core/Input/InputManager.hpp>
#include <Core/State.hpp>
#include <Core/Managers/StateMachine.hpp>
#include <Renderer/EditorCamera.hpp>
#include <EditorState.hpp>
#include <Time.hpp>
#include <Core/Input/Input.hpp>

#include "EditorApp.hpp"

#include "ScriptableBehaviour.hpp"

namespace fox
{
    class Test : public ScriptableBehaviour
    {
        float value;
    public:
        ~Test() override = default;

    protected:
        void on_create() override
        {
            auto& translation = get_component<TransformComponent>()->position;
            translation.x = rand() % 10 - 5.0f;
        }

        void on_update() override
        {
            auto& translation = get_component<TransformComponent>()->position;

            float speed = 5.0f;

            if (Input::IsKeyPressed(Key::A))
                translation.x -= speed * Time::delta_time;
            if (Input::IsKeyPressed(Key::D))
                translation.x += speed * Time::delta_time;
            if (Input::IsKeyPressed(Key::W))
                translation.y += speed * Time::delta_time;
            if (Input::IsKeyPressed(Key::S))
                translation.y -= speed * Time::delta_time;
        }

        void on_destroy() override { }
        int i = 0;
    };
}

EditorApp::EditorApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

EditorApp::~EditorApp() { }

void EditorApp::init()
{
    fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
    sceneManager.PushState(new fox::EditorState());
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new EditorApp(argc, argv);
}