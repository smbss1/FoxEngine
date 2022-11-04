//
// Created by samuel on 02/11/22.
//

#ifndef FOX_LEXER_PANELMANAGER_HPP
#define FOX_LEXER_PANELMANAGER_HPP

#include "Panels/Panel.hpp"
#include "Core/Logger/Logger.hpp"
#include "Hash.hpp"
#include "Events/Signal.hpp"
#include <unordered_map>

namespace fox
{
    class PanelManager
    {
    public:
        struct PanelData
        {
            const char* ID = "";
            const char* Name = "";
            Ref<Panel> EditorPanel = nullptr;
            bool IsOpen = false;
        };

        PanelManager();

        void ImGuiRender();
        void OverlayImGuiRender();

        void Serialize() const;
        void Deserialize();

        PanelData* GetPanelData(uint32_t panelID);

        std::unordered_map<uint32_t, PanelData>& GetPanels();
        const std::unordered_map<uint32_t, PanelData>& GetPanels() const;

        template<typename TPanel>
        Ref<TPanel> AddPanel(const PanelData& panelData)
        {
            static_assert(std::is_base_of<Panel, TPanel>::value, "PanelManager::AddPanel requires TPanel to inherit from EditorPanel");

            uint32_t id = Hash::FNVHash(panelData.ID);
            if (m_Panels.find(id) != m_Panels.end())
            {
                FOX_CORE_ERROR_TAG("PanelManager", "A panel with the id '%' has already been added.", panelData.ID);
                return nullptr;
            }

            m_Panels[id] = panelData;
            return panelData.EditorPanel.As<TPanel>();
        }

        template<typename TPanel, typename... TArgs>
        Ref<TPanel> AddPanel(const char* strID, bool isOpenByDefault, TArgs&&... args)
        {
            return AddPanel<TPanel>(PanelData{ strID, strID, Ref<TPanel>::Create(std::forward<TArgs>(args)...), isOpenByDefault });
        }

        template<typename TPanel, typename... TArgs>
        Ref<TPanel> AddPanel(const char* strID, const char* displayName, bool isOpenByDefault, TArgs&&... args)
        {
            return AddPanel<TPanel>(PanelData{ strID, displayName, Ref<TPanel>::Create(std::forward<TArgs>(args)...), isOpenByDefault });
        }

    private:
        std::unordered_map<uint32_t, PanelData> m_Panels;
    };
}


#endif //FOX_LEXER_PANELMANAGER_HPP
