//
// Created by samuel on 23/06/2021.
//

#include "Core/State.hpp"

namespace fox
{
    State::State(const std::string& debugName)
            : m_strDebugName(debugName)
    {
    }

    World &State::GetWorld()
    {
        return m_pApp->get_world();
    }
}