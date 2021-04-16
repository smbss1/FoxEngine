

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
		std::cout << "OnUpdate!"  << i++ << std::endl;
	}

	virtual void OnDestroy() override
	{

	}
    int i = 0;
};

class ExampleScene : public fox::Scene
{
public:
    ExampleScene()
    {

    }

    ~ExampleScene()
    {

    }

    virtual void on_create() override
    {
        auto e1 = m_oWorld.new_entity();
        e1.add<NativeScript>();
        e1.get<NativeScript>()->bind<Test>();
    }

    void on_destroy() override
    {

    }

    void on_update(Timestep dt) override
    {

    }
};


SandboxApp::SandboxApp()
{
    switch_scene(add_scene(fox::new_ref<ExampleScene>()));
}

SandboxApp::~SandboxApp()
{
    
}

fox::Application* CreateApp()
{
    return new SandboxApp();
}