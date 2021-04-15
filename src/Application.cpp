

#include "Application.hpp"
#include "IGraphic.hpp"
#include "GraphicLibLoader.hpp"
#include "Input.hpp"
#include "FoxEngine.hpp"


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
		std::cout << "OnUpdate!" << std::endl;
	}

	virtual void OnDestroy() override
	{

	}
};

namespace fox
{
    Application::Application()
    {
        m_bIsRunning = true;
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        float fFixedDeltaTime = 0;
        float fFixedTimeStep = 1.0f / 45.0f;
        float fDeltaTime = 0;
        IGraphic* pGraphicContext = nullptr;
        GraphicLibLoader loader("graphics/fox_sfml" + std::string(DL_EXT));


        oWorld.system<NativeScript>().kind(ecs::OnUpdate)
            .each([](Entity e, NativeScript& script) {
                if (!script.m_pInstance) {
                    script.m_pInstance = script.InstantiateScript();
                    script.m_pInstance->m_pEntity = e;
                    script.m_pInstance->OnCreate();
                }
                script.m_pInstance->OnUpdate();
        });

        // oWorld.system<Transform>().kind(ecs::OnAdd)
        //     .each([](Entity e, Transform& transform) {
        //         std::cout << "Add new Transform" << std::endl;
        // });
        
        auto e1 = oWorld.new_entity();
        e1.add<NativeScript>();
        e1.get<NativeScript>()->bind<Test>();

        Input input;
        pGraphicContext = loader.get<IGraphic>();
        pGraphicContext->init();
        while (m_bIsRunning)
        {
            pGraphicContext->poll_event(input);
            fFixedDeltaTime += fDeltaTime;
            while (fFixedDeltaTime >= fFixedTimeStep)
            {
                fFixedDeltaTime -= fFixedTimeStep;
            }

            oWorld.run_phase(ecs::PreUpdate);
            oWorld.run_phase(ecs::OnUpdate);
            oWorld.run_phase(ecs::OnValidate);
            oWorld.run_phase(fox::ecs::PostUpdate);
            oWorld.run_phase(fox::ecs::PreStore);
            oWorld.run_phase(fox::ecs::OnStore);
            pGraphicContext->draw();

            if (pGraphicContext->quit_requested()) {
                m_bIsRunning = false;
            }
        }
        // pGraphicContext->shutdown();
    }
}