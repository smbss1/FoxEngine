//
// Created by samuel on 09/10/22.
//

#ifndef FOX_LEXER_SETTINGSPANEL_HPP
#define FOX_LEXER_SETTINGSPANEL_HPP

#include "Panel.hpp"
#include "common.hpp"

namespace fox
{
    class Scene;
    struct OnContextChangeEvent;

    class SettingsPanel : public Panel
    {
    public:
        SettingsPanel();
        void OnImGui() override;
        void OnOverlayRender() override;

    private:
        void OnContextChangeChange(const OnContextChangeEvent& event);


        bool m_ShowPhysicsColliders = false;
        Ref<Scene> m_pContext;

    };
}


#endif //FOX_LEXER_SETTINGSPANEL_HPP
