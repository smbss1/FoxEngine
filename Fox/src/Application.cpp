

#include "IGraphic.hpp"
#include "GraphicLibLoader.hpp"
#include "Input.hpp"
#include "Application.hpp"

namespace fox
{
    Application::Application(int ac, char** av)
    {
        m_bIsRunning = true;
        m_pSceneManager = new_scope<SceneManager>();
        m_pResourceManager = new_scope<ResourceManager>();
    }

    Application::~Application()
    {
    }

    void Application::init()
    {
    }

    void Application::run()
    {
        init();
        float fFixedDeltaTime = 0;
        float fFixedTimeStep = 1.0f / 45.0f;
        float fDeltaTime = 0;
        IGraphic* pGraphicContext = nullptr;
        GraphicLibLoader loader("Fox/graphics/fox_sfml" + std::string(DL_EXT));

        Input input;
        pGraphicContext = loader.get<IGraphic>();
        pGraphicContext->init(*this);
        m_pSceneManager->start();
        while (m_bIsRunning)
        {
            pGraphicContext->poll_event(input);
            fFixedDeltaTime += fDeltaTime;
            while (fFixedDeltaTime >= fFixedTimeStep)
            {
                fFixedDeltaTime -= fFixedTimeStep;
            }

            m_pSceneManager->update(0.0f);
            pGraphicContext->draw(*this);

            if (pGraphicContext->quit_requested())
                m_bIsRunning = false;
        }
        pGraphicContext->shutdown(*this);
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

    // ref<Scene> Application::get_active()
    // {
    //     return m_pSceneManager->get_active();
    // }

    // ResourceManager& Application::get_resource_manager() const
    // {
    //     return *m_pResourceManager;
    // }
}
