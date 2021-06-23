//
// Created by samuel on 23/06/2021.
//

#ifndef FOXENGINE_STATE_HPP
#define FOXENGINE_STATE_HPP

#include <string>

namespace fox
{
    class State
    {
    public:
        State(const std::string &name = "State");

        virtual ~State() = default;

        virtual void OnAttach()
        {}

        virtual void OnDetach()
        {}

        virtual void OnUpdate(float ts)
        {}

        virtual void OnImGuiRender()
        {}

//        virtual void OnEvent(Event &event)
//        {}

        const std::string &GetName() const
        { return m_strDebugName; }

    protected:
        std::string m_strDebugName;
    };
}

#endif //FOXENGINE_STATE_HPP
