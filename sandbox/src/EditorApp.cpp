

#include <Core/Input/InputManager.hpp>
#include <Core/Logger/Logger.hpp>
#include "SandboxApp.hpp"
#include "ScriptableBehaviour.hpp"
#include "NativeScript.hpp"
#include "Animator.hpp"

class Test : public ScriptableBehaviour
{
    float value;
public:
	virtual ~Test() {}

protected:
	virtual void on_create() override
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
        get_world().new_entity().add<NativeScript>(Test());
    }

    void on_destroy(fox::Application& app) override { }
    void on_update(fox::Application& app) override
    {
    }
};

EditorApp::EditorApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

EditorApp::~EditorApp() { }

void EditorApp::init()
{
    fox::SceneManager& sceneManager = get<fox::SceneManager>().value();
    sceneManager.add<ExampleScene>("Example");
    sceneManager.switch_to("Example");
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new EditorApp(argc, argv);
}