

#include <Core/Managers/PluginManager.hpp>
#include <chrono>
#include <Time.hpp>
#include <Utils/FileSystem.hpp>
#include <Core/Managers/StateMachine.hpp>
#include <Core/Managers/ResourceManager.hpp>
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommand.hpp>
#include <Core/Input/Input.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Renderer2D.hpp>
#include "Core/Input/InputManager.hpp"
#include "Core/Application.hpp"
#include "Core/Logger/Logger.hpp"
#include "json.hpp"

namespace fox
{
    Application::Application(int ac, char** av)
    {
        m_bIsRunning = true;
        set<StateMachine>(*this);
        set<ResourceManager>();
//        set<InputManager>();
        set<TimeInfo>();
    }

    Application::~Application()
    {
        remove<ResourceManager>();
        remove<StateMachine>();
        Renderer2D::Shutdown();
        RendererCommand::Shutdown();
    }

    void Application::init()
    {
    }

    void Application::quit()
    {
        m_bIsRunning = false;
    }

    void Application::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(FOX_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(FOX_BIND_EVENT_FN(Application::OnWindowResize));
        auto& oStateMachine = get<StateMachine>().value();

        for (auto it = oStateMachine.rbegin(); it != oStateMachine.rend(); ++it)
        {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    void Application::LoadConfig()
    {
        fox::info("Starting load config.json....");
        json::Value oConfigTemp;
        std::string out;
        if (fox::file::ReadFile(FOX_PLUGIN_DIRECTORY"config.json", out))
            oConfigTemp = json::parse(out);
        else
            fox::error("Cannot read config.json");
        if (!oConfigTemp.is_null()) {
            m_oConfigFile = new_scope<json::Value>(oConfigTemp);
            fox::info("config.json load successfully");
        }
        else
            fox::error("Wrong configuration format for 'config.json'");
    }

    void Application::run()
    {
        fox::info("Starting Application...");
        float fFixedDeltaTime = 0;
        float fFixedTimeStep = 1.0f / 45.0f;
        float fDeltaTime;
        auto lastUpdate = std::chrono::high_resolution_clock::now();

        LoadConfig();

        PluginManager& plugin_manager = get_or_create<fox::PluginManager>().value();
        plugin_manager.FindAndLoadPlugins(FOX_PLUGIN_DIRECTORY + (*m_oConfigFile)["plugins directory"].get<std::string>());
        if(plugin_manager.GetGraphics().GetCount() <= 0)
            throw std::runtime_error("Cannot run the application because no Graphics Plugins found");

        if(plugin_manager.GetWindowPlugin() == nullptr)
            throw std::runtime_error("Cannot run the application because no Window Plugin found");

        GraphicPlugin& graphic_ctx = plugin_manager.GetGraphics().GetPlugin(0);
        RendererAPI::SetGraphicPlugin(&graphic_ctx);

        m_pWindow = plugin_manager.GetWindowPlugin()->CreateWindow(WindowProps());
        m_pWindow->SetEventCallback(FOX_BIND_EVENT_FN(Application::OnEvent));
        Input::SetWindow(m_pWindow.get());

        plugin_manager.InitializePlugins(*this);

        RendererCommand::SetRendererAPI(graphic_ctx.CreateRenderer());
        Renderer::Init();

        init();
        fox::info("Application is running");
        while (m_bIsRunning)
        {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = now - lastUpdate;
            lastUpdate = now;

            fDeltaTime = elapsed.count();
            Time::delta_time = fDeltaTime;
            if (Time::delta_time > 0.25)
                Time::delta_time = 0.25;
            Time::time += fDeltaTime;
            get<TimeInfo>()->delta_time = Time::delta_time;
            get<TimeInfo>()->time = Time::time;

            fFixedDeltaTime += fDeltaTime;
            while (fFixedDeltaTime >= fFixedTimeStep)
            {
                // get<SceneManager>()->fix_update();
                Time::fixed_delta_time = fFixedDeltaTime;
                get<TimeInfo>()->time = Time::fixed_delta_time;
                fFixedDeltaTime -= fFixedTimeStep;
            }
            Time::factor_physics = fFixedDeltaTime / fFixedTimeStep;
            get<TimeInfo>()->factor_physics = Time::factor_physics;

            if (!m_bIsMinimized)
            {
                get<StateMachine>()->Update();


                graphic_ctx.update();
            }
            m_pWindow->OnUpdate();
        }
    }

    Window* Application::GetWindow() const
    {
        return m_pWindow.get();
    }

    json::Value &Application::GetConfigs() const
    {
        return *m_oConfigFile;
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_bIsRunning = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_bIsMinimized = true;
            return false;
        }

        m_bIsMinimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }

    std::unordered_map<size_t, ScriptCreator> &Application::GetScripts()
    {
        return m_vScripts;
    }

    void Application::SetScriptsArray(const std::unordered_map<size_t, ScriptCreator> &scripts)
    {
        m_vScripts = scripts;
    }
}
