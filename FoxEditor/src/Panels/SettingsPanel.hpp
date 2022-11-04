//
// Created by samuel on 09/10/22.
//

#ifndef FOX_LEXER_SETTINGSPANEL_HPP
#define FOX_LEXER_SETTINGSPANEL_HPP

#include "Panel.hpp"
#include "common.hpp"
#include "Core/Project.hpp"

namespace fox
{
    class Scene;
    struct OnContextChangeEvent;

    class SettingsPanel : public Panel
    {
    public:
        SettingsPanel();
        void OnImGui(bool& isOpen) override;
        void OnOverlayRender() override;

    private:
        void OnContextChangeChange(const OnContextChangeEvent& event);
        void ImGuiPhysicsSettings();

        bool m_ShowPhysicsColliders = false;
        Ref<Scene> m_pContext;
        int m_SelectedLayer = -1;

        Ref<Project> m_Project;
        bool m_SerializeProject = false;
    };
}


#endif //FOX_LEXER_SETTINGSPANEL_HPP
