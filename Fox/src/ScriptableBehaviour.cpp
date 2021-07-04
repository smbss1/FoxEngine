//
// Created by samuel on 04/07/2021.
//

#include "Core/Application.hpp"
#include "Components/ScriptableBehaviour.hpp"

void ScriptableBehaviour::set_app(fox::Application& app)
{
    m_pApp = &app;
}

fox::Application& ScriptableBehaviour::get_app()
{
    return *m_pApp;
}