//
// Created by samuel on 23/06/2021.
//

#include <algorithm>
#include "Core/Managers/StateMachine.hpp"

namespace fox
{

    StateMachine::StateMachine(Application &app) : m_oApp(app)
    {
    }

    StateMachine::~StateMachine()
    {
        for (State *layer : m_vStates) {
            layer->OnExit();
            delete layer;
        }
    }

    void StateMachine::PushState(State* layer)
    {
        m_vStates.emplace(m_vStates.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
        layer->SetApplication(m_oApp);
        layer->OnEnter();
    }

    void StateMachine::PushOverlay(State* overlay)
    {
        m_vStates.emplace_back(overlay);
        overlay->SetApplication(m_oApp);
        overlay->OnEnter();
    }

    void StateMachine::PopState(State* layer)
    {
        auto it = std::find(m_vStates.begin(), m_vStates.begin() + m_LayerInsertIndex, layer);
        if (it != m_vStates.begin() + m_LayerInsertIndex) {
            layer->OnExit();
            m_vStates.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void StateMachine::PopOverlay(State* overlay)
    {
        auto it = std::find(m_vStates.begin() + m_LayerInsertIndex, m_vStates.end(), overlay);
        if (it != m_vStates.end()) {
            overlay->OnExit();
            m_vStates.erase(it);
        }
    }

    void StateMachine::FixUpdate()
    {

    }

    void StateMachine::Update()
    {
        for (auto& state : m_vStates)
        {
            state->OnUpdate();
        }
    }

    void StateMachine::UpdateImGui()
    {
        for (auto& state : m_vStates)
        {
            state->OnImGui();
        }
    }
}