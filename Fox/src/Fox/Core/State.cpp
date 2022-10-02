//
// Created by samuel on 23/06/2021.
//

#include "Core/State.hpp"
#include "Core/Application.hpp"

namespace fox
{
    State::State(const std::string& debugName)
            : m_strDebugName(debugName)
    {
    }

    void State::SetApplication(fox::Application& app)
    {
        m_pApp = &app;
    }

    Application& State::GetApp() const
    {
        return *m_pApp;
    }

//    World &State::GetWorld()
//    {
//        return m_pApp->get_world();
//    }
}