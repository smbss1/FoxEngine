
#include <Utils/FileSystem.hpp>
#include "Save for later/StateMachine.hpp"
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommand.hpp>
#include <Core/Input/Input.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Renderer2D.hpp>
#include "Core/Application.hpp"
#include "Core/Logger/Logger.hpp"
#include "json.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Utils/PlatformUtils.hpp"

namespace fox
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& specification)
            : m_Specification(specification)
    {
        FOX_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_bIsRunning = true;

        // Set working directory here
        if (!m_Specification.WorkingDirectory.empty())
            std::filesystem::current_path(m_Specification.WorkingDirectory);

        m_pWindow = Window::Create(WindowProps(m_Specification.Name));
        m_pWindow->SetEventCallback(FOX_BIND_EVENT_FN(Application::OnEvent));
        Input::SetWindow(m_pWindow.get());

        Renderer::Init();
        ScriptEngine::Init();

        // Add the ImGui state of GUI
        m_ImGuiLayer = new ImGuiLayer;
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        ScriptEngine::Shutdown();
        Renderer2D::Shutdown();
        RendererCommand::Shutdown();
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::Close()
    {
        m_bIsRunning = false;
    }

    void Application::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(FOX_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(FOX_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
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
        if (fox::file::ReadFile(std::string(FOX_PLUGIN_DIRECTORY) + "config.json", out))
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

    void Application::Run()
    {
        fox::info("Starting Application...");

        // LoadConfig();

        // PluginManager& plugin_manager = get_or_create<fox::PluginManager>().value();
        // plugin_manager.FindAndLoadPlugins(std::string(FOX_PLUGIN_DIRECTORY) + (*m_oConfigFile)["plugins directory"].get<std::string>());
        // if(plugin_manager.GetGraphics().GetCount() <= 0)
        //     throw std::runtime_error("Cannot Run the application because no Graphics Plugins found");

        // if(plugin_manager.GetWindowPlugin() == nullptr)
        //     throw std::runtime_error("Cannot Run the application because no Window Plugin found");

        // GraphicPlugin& graphic_ctx = plugin_manager.GetGraphics().GetPlugin(0);
        // RendererAPI::SetGraphicPlugin(&graphic_ctx);

        // m_pWindow = plugin_manager.GetWindowPlugin()->CreateWindow(WindowProps());

        // m_pWindow = Window::Create(WindowProps());
        // m_pWindow->SetEventCallback(FOX_BIND_EVENT_FN(Application::OnEvent));
        // Input::SetWindow(m_pWindow.get());

        // plugin_manager.InitializePlugins(*this);

        fox::info("Application is running");
        while (m_bIsRunning)
        {
            float time = Time::GetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_bIsMinimized)
            {
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(timestep);

                m_ImGuiLayer->Begin();
                {

                    for (Layer* layer : m_LayerStack)
                        layer->OnImGuiRender();
                }
                m_ImGuiLayer->End();
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

//    std::unordered_map<size_t, ScriptCreator> &Application::GetScripts()
//    {
//        return m_vScripts;
//    }
//
//    void Application::SetScriptsArray(const std::unordered_map<size_t, ScriptCreator> &scripts)
//    {
//        m_vScripts = scripts;
//    }
}
