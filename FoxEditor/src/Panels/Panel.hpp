//
// Created by samuel on 09/10/22.
//

#ifndef FOXENGINE_PANEL_HPP
#define FOXENGINE_PANEL_HPP

#include "Core/Ref.hpp"

namespace fox
{
    class Panel : public RefCounted
    {
    public:
        virtual void OnImGui() = 0;
        virtual void OnOverlayRender() {}

        bool EnableOverlay = false;
    };
}
#endif //FOXENGINE_PANEL_HPP
