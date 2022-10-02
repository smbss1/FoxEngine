
#ifndef FOX_APP_HPP_
#define FOX_APP_HPP_

#include <Renderer/OrthographicCamera.hpp>
#include <Events/ApplicationEvent.hpp>
#include <Components/NativeScript.hpp>

#include "FoxEcs.hpp"
#include "Core/Managers/ResourceManager.hpp"
#include "Core.hpp"
#include "Window.hpp"
#include "ImGui/ImGuiState.hpp"

// int main(int argc, char** argv);

namespace fox
{
    namespace json
    {
        class Value;
    }

    class Application
    {
    public:
        Application(int ac, char** av);
        virtual ~Application();
    
        virtual void init();
        void run();
        void quit();
        void OnEvent(Event& e);

        void LoadConfig();

        template <typename T, typename... Args>
        T& set(Args&&... args)
        {
            return m_vAny.set<T>(args...);
        }

        template <typename T>
        fox::Option<T&> get()
        {
            return m_vAny.get<T>();
        }

        template <typename T, typename... Args>
        fox::Option<T&> get_or_create(Args&&... args)
        {
            auto option = m_vAny.get<T>();
            if (!option)
                m_vAny.set<T>(args...);
            return m_vAny.get<T>();
        }

        template <typename T>
        fox::Option<const T&> get() const
        {
            return m_vAny.get<T>();
        }

        template <typename T>
        void remove()
        {
            m_vAny.remove<T>();
        }

        Window* GetWindow() const;
        json::Value& GetConfigs() const;

        bool IsWindowMinized() const { return m_bIsMinimized; }

        void SetScriptsArray(const std::unordered_map<size_t, ScriptCreator>& scripts);

        std::unordered_map<size_t, ScriptCreator>& GetScripts();

    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        bool m_bIsRunning;
        bool m_bIsMinimized = false;

        ImGuiState* m_pImGuiState = nullptr;

    protected:
        fox::AnyContainer m_vAny;
        scope<json::Value> m_oConfigFile;
        ref<Window> m_pWindow;


        std::unordered_map<size_t, ScriptCreator> m_vScripts;

        // friend int ::main(int argc, char** argv);
    };

    Application* CreateApp(int argc, char** argv);
}

#endif