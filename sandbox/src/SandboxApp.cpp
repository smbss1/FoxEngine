

#include <Core/Input/InputManager.hpp>
#include <Core/Logger/Logger.hpp>
#include "SandboxApp.hpp"
#include "ScriptableBehaviour.hpp"
#include "NativeScript.hpp"

class Test : public ScriptableBehaviour
{
public:
	virtual ~Test() {}

protected:
	virtual void on_create() override
	{
		std::cout << "OnCreate!" << std::endl;
	}

	virtual void on_update() override
	{
		// std::cout << "OnUpdate!"  << i++ << std::endl;
        fox::InputManager& input = get_app().get<fox::InputManager>().value();

        if (input.GetKeyDown(fox::Key::A))
        {
            fox::info("A pressed");
        }

        if (input.GetMouseButtonDown(fox::MouseButton::ButtonLeft))
        {
            fox::info("Left pressed");
        }

        if (input.GetMouseButtonUp(fox::MouseButton::ButtonLeft))
        {
            fox::info("Left released");
        }

        if (input.GetMouseButtonDown(fox::MouseButton::ButtonRight))
        {
            fox::info("Right pressed");
        }
//        fox::info("%", input.GetMousePosition());
//        fox::info("Scroll: %", input.GetMouseScroll());
	}

	virtual void on_destroy() override { }
    int i = 0;
};

class ExampleScene : public fox::Scene
{
public:
    ExampleScene() { }
    ExampleScene(const std::string& name) : fox::Scene(name) { }
    ~ExampleScene() { }

    virtual void on_create(fox::Application& app) override { }

    virtual void on_enable(fox::Application& app) override
    {
        auto e1 = get_world().new_entity();
        e1.add<NativeScript>(Test())
          .add<SpriteRenderer>("", "MySprite")
          .add<fox::Transform>();
        e1.get<fox::Transform>()->set_pos(45, 25);
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
    sceneManager.add<ExampleScene>("Example");
    sceneManager.switch_to("Example");
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new SandboxApp(argc, argv);
}