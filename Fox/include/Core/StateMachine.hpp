//
// Created by samuel on 23/06/2021.
//

#ifndef FOXENGINE_STATEMACHINE_HPP
#define FOXENGINE_STATEMACHINE_HPP

#include <vector>
#include "Core/State.hpp"

namespace fox
{
    class StateMachine
    {
    public:
        explicit StateMachine(Application& app);

        ~StateMachine();

        void PushState(State *state);

        void PushOverlay(State *overlay);

        void PopState(State *state);

        void PopOverlay(State *overlay);

        void FixUpdate();
        void Update();
        void UpdateImGui();

        std::vector<State *>::iterator begin()
        { return m_vStates.begin(); }

        std::vector<State *>::iterator end()
        { return m_vStates.end(); }

        std::vector<State *>::reverse_iterator rbegin()
        { return m_vStates.rbegin(); }

        std::vector<State *>::reverse_iterator rend()
        { return m_vStates.rend(); }

        [[nodiscard]] std::vector<State *>::const_iterator begin() const
        { return m_vStates.begin(); }

        [[nodiscard]] std::vector<State *>::const_iterator end() const
        { return m_vStates.end(); }

        [[nodiscard]] std::vector<State *>::const_reverse_iterator rbegin() const
        { return m_vStates.rbegin(); }

        [[nodiscard]] std::vector<State *>::const_reverse_iterator rend() const
        { return m_vStates.rend(); }

    private:
        std::vector<State *> m_vStates;
        unsigned int m_LayerInsertIndex = 0;
        Application& m_oApp;
    };
}


#endif //FOXENGINE_STATEMACHINE_HPP
