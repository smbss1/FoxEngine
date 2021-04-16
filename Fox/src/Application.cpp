

#include "IGraphic.hpp"
#include "GraphicLibLoader.hpp"
#include "Input.hpp"
#include "Application.hpp"

namespace fox
{
    Application::Application()
    {
        m_bIsRunning = true;
        m_pSceneManager = new_scope<SceneManager>();
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
        GraphicLibLoader loader("Fox/graphics/fox_sfml" + std::string(DL_EXT));

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

            m_pSceneManager->update(0.0f);
            pGraphicContext->draw();

            if (pGraphicContext->quit_requested())
                m_bIsRunning = false;
        }
        pGraphicContext->shutdown();
    }

    SceneId Application::add_scene(ref<Scene> pScene)
    {
        return m_pSceneManager->add(pScene);
    }

    void Application::switch_scene(SceneId id)
    {
        m_pSceneManager->switch_to(id);
    }

    void Application::remove_scene(SceneId id)
    {
        m_pSceneManager->remove(id);
    }
}
