
#ifndef FOX_APP_HPP_
#define FOX_APP_HPP_

#include <Renderer/OrthographicCamera.hpp>
#include <Events/ApplicationEvent.hpp>
#include "FoxEcs.hpp"
#include "Core/Managers/SceneManager.hpp"
#include "Core/Managers/ResourceManager.hpp"
#include "Core.hpp"
#include "Window.hpp"

namespace fox
{
    namespace json
    {
        class Value;
    }
    class FOX_API Application
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

        World& get_world()
        {
            return *m_pWorld;
        }

    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        bool m_bIsRunning;
        bool m_bIsMinimized = false;

    protected:
        std::unique_ptr<World> m_pWorld;
        fox::AnyContainer m_vAny;
        scope<json::Value> m_oConfigFile;

        ref<Window> m_pWindow;
    };
}

#endif