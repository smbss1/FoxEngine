//
// Created by samuel on 09/10/22.
//

#ifndef FOX_LEXER_STATSPANEL_HPP
#define FOX_LEXER_STATSPANEL_HPP

#include "Panel.hpp"

namespace fox
{
    class StatsPanel : public Panel
    {
        void OnImGui(bool& isOpen) override;
    };
}


#endif //FOX_LEXER_STATSPANEL_HPP
