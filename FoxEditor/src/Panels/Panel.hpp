//
// Created by samuel on 09/10/22.
//

#ifndef FOXENGINE_PANEL_HPP
#define FOXENGINE_PANEL_HPP

namespace fox
{
    class Panel
    {
    public:
        virtual void OnImGui() = 0;
    };
}
#endif //FOXENGINE_PANEL_HPP
