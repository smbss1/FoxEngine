//
// Created by samuel on 05/10/22.
//

#ifndef FOXENGINE_LAYER_HPP
#define FOXENGINE_LAYER_HPP

#include "Core/Base.hpp"
#include "Core/Timestep.hpp"
#include "Events/Event.hpp"

#include <string>

namespace fox
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnImGuiRender() {}
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };

}

#endif //FOXENGINE_LAYER_HPP
