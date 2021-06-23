//
// Created by samuel on 23/06/2021.
//

#include <algorithm>
#include "Core/Managers/StateMachine.hpp"

namespace fox
{
    StateMachine::~StateMachine()
    {
        for (State *layer : m_vStates) {
            layer->OnDetach();
            delete layer;
        }
    }

    void StateMachine::PushState(State* layer)
    {
        m_vStates.emplace(m_vStates.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
    }

    void StateMachine::PushOverlay(State* overlay)
    {
        m_vStates.emplace_back(overlay);
    }

    void StateMachine::PopState(State* layer)
    {
        auto it = std::find(m_vStates.begin(), m_vStates.begin() + m_LayerInsertIndex, layer);
        if (it != m_vStates.begin() + m_LayerInsertIndex) {
            layer->OnDetach();
            m_vStates.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void StateMachine::PopOverlay(State* overlay)
    {
        auto it = std::find(m_vStates.begin() + m_LayerInsertIndex, m_vStates.end(), overlay);
        if (it != m_vStates.end()) {
            overlay->OnDetach();
            m_vStates.erase(it);
        }
    }
}