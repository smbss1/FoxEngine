
#include <Utils/FileSystem.hpp>
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommand.hpp>
#include <Core/Input/Input.hpp>
#include <Renderer/Renderer.hpp>
#include "Core/Application.hpp"
#include "Core/Logger/Logger.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Utils/PlatformUtils.hpp"
#include "Renderer/Commands.hpp"

namespace fox
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& specification)
            : m_Specification(specification)
    {
        FOX_CORE_INFO("Starting Application...");
        FOX_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // Set working directory here
        if (!m_Specification.WorkingDirectory.empty())
        {
            fs::current_path(m_Specification.WorkingDirectory);
        }

        m_pWindow = Window::Create(WindowProps(m_Specification.Name));
        m_pWindow->SetEventCallback(FOX_BIND_EVENT_FN(Application::OnEvent));
        Input::SetWindow(m_pWindow.get());

        Renderer::Init();
        ScriptEngine::Init(specification.ScriptSetting);

        // Add the ImGui state of GUI
        m_ImGuiLayer = new ImGuiLayer;
        PushOverlay(m_ImGuiLayer);

        m_bIsRunning = true;
    }

    Application::~Application()
    {
        Input::SetWindow(nullptr);
		m_MainThreadQueue.clear();
        ScriptEngine::Shutdown();
        Renderer::WaitAndRender();
        Renderer::Shutdown();

        m_LayerStack.~LayerStack();
        m_pWindow = nullptr;
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

    void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(function);
	}

    void Application::Run()
    {
        FOX_CORE_INFO("Application is running");
        while (m_bIsRunning)
        {
            ExecuteMainThreadQueue();

            if (!m_bIsMinimized)
            {
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(m_TimeStep);

                Application* app = this;
                RendererCommand::Push<FunctionCmd>([app]{ app->m_ImGuiLayer->Begin(); });
                RendererCommand::Push<FunctionCmd>([app]{
                    for (Layer* layer : app->m_LayerStack)
                        layer->OnImGuiRender();
                });
                RendererCommand::Push<FunctionCmd>([app]{ app->m_ImGuiLayer->End(); });
            }
            Input::OnUpdate();
            Renderer::WaitAndRender();
            m_pWindow->OnUpdate();

            float time = Time::GetTime();
            m_Frametime = time - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_Frametime, 0.0333f);
            Time::delta_time = m_TimeStep;
            m_LastFrameTime = time;
        }
    }

    Window* Application::GetWindow() const
    {
        return m_pWindow.get();
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

    void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}
}
