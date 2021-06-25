
#ifndef FOX_APP_HPP_
#define FOX_APP_HPP_

#include "FoxEcs.hpp"
#include "Core/Managers/SceneManager.hpp"
#include "Core/Managers/ResourceManager.hpp"
#include "Core.hpp"

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
        bool m_bIsRunning;

    protected:
        std::unique_ptr<World> m_pWorld;
        fox::AnyContainer m_vAny;
        scope<json::Value> m_oConfigFile;
    };
}

#endif