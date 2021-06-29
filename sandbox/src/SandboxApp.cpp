
#include <FoxEngine.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <FirstTriangle.hpp>
#include <Example2D.hpp>
#include <SpriteSheet.hpp>

#include "SandboxApp.hpp"
#include "ScriptableBehaviour.hpp"
#include "NativeScript.hpp"
#include "Animator.hpp"

#include "FirstTriangle.hpp"

class Test : public ScriptableBehaviour
{
    float value;
public:
	~Test() override = default;

protected:
	void on_create() override
	{
		std::cout << "OnCreate!" << std::endl;

        add_component<fox::Transform>(Vec2(45, 25));
        add_component<SpriteRenderer>("", "MySprite", 12);
        auto& anim = add_component<Animator>().add_anim_from_file("sandbox/assets/animation.json");
        anim.signal(4, [&]{ fox::info("Value: %", value); });
        anim.signal(4.1, [&]{ fox::info("Value: %", value); });
        anim.signal(4.5, [&]{ fox::info("Value: %", value); });
        anim.signal(5.00001, [&]{ fox::info("Value: %", value); });
        anim.signal(6.0001, [&]{ fox::info("Value: %", value); });
        anim.signal(7.001, [&]{ fox::info("Value: %", value); });
        anim.signal(8.01, [&]{ fox::info("Value: %", value); });
        anim.get<float>(0).apply(value);
	}

	void on_update() override
	{
		// std::cout << "OnUpdate!"  << i++ << std::endl;
//        fox::InputManager& input = get_app().get<fox::InputManager>().value();
//
//        if (input.GetKeyDown(fox::Key::A))
//        {
//            fox::info("A pressed");
//        }
//
//        if (input.GetMouseButtonDown(fox::MouseButton::ButtonLeft))
//        {
//            fox::info("Left pressed");
//        }
//
//        if (input.GetMouseButtonUp(fox::MouseButton::ButtonLeft))
//        {
//            fox::info("Left released");
//        }
//
//        if (input.GetMouseButtonDown(fox::MouseButton::ButtonRight))
//        {
//            fox::info("Right pressed");
//        }
//        fox::info("%", input.GetMousePosition());
//        fox::info("Scroll: %", input.GetMouseScroll());
	}

	void on_destroy() override { }
    int i = 0;
};

SandboxApp::SandboxApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

SandboxApp::~SandboxApp() { }

void SandboxApp::init()
{
    fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
//    sceneManager.PushState(new FirstTriangleState);
//    sceneManager.PushState(new Example2DState);
    sceneManager.PushState(new SpriteSheetState);
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new SandboxApp(argc, argv);
}