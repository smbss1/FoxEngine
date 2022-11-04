
#ifndef FOX_APP_HPP_
#define FOX_APP_HPP_

#include "Core/Base.hpp"

#include "Core/Window.hpp"
#include "Core/LayerStack.hpp"
#include "Events/Event.hpp"
#include "Events/ApplicationEvent.hpp"

#include "Core/Timestep.hpp"

#include "ImGui/ImGuiLayer.hpp"
#include "Assert.hpp"
#include "Scripting/ScriptEngine.hpp"

namespace fox
{
    // namespace json
    // {
    //     class Value;
    // }

    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](int index) const
        {
            FOX_ASSERT(index < Count);
            return Args[index];
        }
    };

    struct ApplicationSpecification
    {
        std::string Name = "Hazel Application";
        std::string WorkingDirectory;
        ApplicationCommandLineArgs CommandLineArgs;

        ScriptEngineSetting ScriptSetting;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& specification);
        virtual ~Application();

        void Run();
        void Close();
        void OnEvent(Event& e);

        void SubmitToMainThread(const std::function<void()>& function);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        // void LoadConfig();

        Window* GetWindow() const;
        // json::Value& GetConfigs() const;
        static Application& Get() { return *s_Instance; }

        bool IsWindowMinized() const { return m_bIsMinimized; }
        ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
        const ApplicationSpecification& GetSpecs() { return m_Specification; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        void ExecuteMainThreadQueue();

        bool m_bIsRunning;
        bool m_bIsMinimized = false;

        ApplicationSpecification m_Specification;
        scope<Window> m_pWindow;
        float m_LastFrameTime = 0.0f;
        ImGuiLayer* m_ImGuiLayer = nullptr;
        LayerStack m_LayerStack;
        // scope<json::Value> m_oConfigFile;

        std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

    private:
        static Application* s_Instance;
        Timestep m_Frametime;
        Timestep m_TimeStep;


//        std::unordered_map<size_t, ScriptCreator> m_vScripts;
    };

    Application* CreateApp(ApplicationCommandLineArgs args);
}

#endif
