

#include "SandboxApp.hpp"

class Test : public ScriptableEntity
{
public:
	virtual ~Test() {}

protected:
	virtual void OnCreate() override
	{
		std::cout << "OnCreate!" << std::endl;
	}

	virtual void OnUpdate() override
	{
		// std::cout << "OnUpdate!"  << i++ << std::endl;
	}

	virtual void OnDestroy() override { }
    int i = 0;
};

class ExampleScene : public fox::Scene
{
public:
    ExampleScene() { }
    ~ExampleScene() { }

    virtual void on_create() override { }

    virtual void on_start() override
    {
        auto e1 = m_oWorld.new_entity();
        e1.add<NativeScript>()
          .add<SpriteRenderer>("", "MySprite")
          .add<Transform>();
        e1.get<NativeScript>()->bind<Test>();
        e1.get<Transform>()->set_pos(45, 25);
    }

    void on_destroy() override { }
    void on_update(Timestep dt) override { }
};

SandboxApp::SandboxApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

SandboxApp::~SandboxApp() { }

void SandboxApp::init()
{
    switch_scene(add_scene(fox::new_ref<ExampleScene>()));
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new SandboxApp(argc, argv);
}