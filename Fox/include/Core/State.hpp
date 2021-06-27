//
// Created by samuel on 23/06/2021.
//

#ifndef FOXENGINE_STATE_HPP
#define FOXENGINE_STATE_HPP

#include <string>
#include "Application.hpp"

namespace fox
{
    class State
    {
    public:
        State(const std::string &name = "State");

        virtual ~State() = default;

        virtual void OnEnter()
        {}

        virtual void OnExit()
        {}

        virtual void OnEvent(Event& event)
        {}

        virtual void OnUpdate()
        {}

        virtual void OnImGui()
        {}

//        virtual void OnEvent(Event &event)
//        {}

        const std::string &GetName() const
        { return m_strDebugName; }

        /**
         * @brief Get the ECS World
         * @return ECS World
         */
        World& GetWorld();

        void SetApplication(fox::Application& app)
        {
            m_pApp = &app;
        }

        fox::Application& GetApp() const
        {
            return *m_pApp;
        }

    protected:
        std::string m_strDebugName;
        fox::Application* m_pApp = nullptr;
    };
}

#endif //FOXENGINE_STATE_HPP
