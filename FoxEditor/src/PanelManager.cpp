//
// Created by samuel on 02/11/22.
//

#include <yaml-cpp/yaml.h>
#include "PanelManager.hpp"
#include "Utils/FileSystem.hpp"
#include "Core/Assert.hpp"

namespace fox
{
    PanelManager::PanelManager()
    {
    }

    PanelManager::PanelData* PanelManager::GetPanelData(uint32_t panelID)
    {
        if (m_Panels.find(panelID) == m_Panels.end())
            return nullptr;

        return &m_Panels.at(panelID);
    }

    std::unordered_map<uint32_t, PanelManager::PanelData>& PanelManager::GetPanels()
    {
        return m_Panels;
    }

    const std::unordered_map<uint32_t, PanelManager::PanelData>& PanelManager::GetPanels() const
    {
        return m_Panels;
    }

    void PanelManager::ImGuiRender()
    {
        for (auto& it : m_Panels)
        {
            PanelData& data = it.second;
            bool closedThisFrame = false;

            if (data.IsOpen)
            {
                data.EditorPanel->OnImGui(data.IsOpen);
                closedThisFrame = !data.IsOpen;
            }

            if (closedThisFrame)
                Serialize();
        }
    }

    void PanelManager::OverlayImGuiRender()
    {
        for (auto it : m_Panels)
        {
            PanelData& data = it.second;
            if (data.EditorPanel->EnableOverlay)
                data.EditorPanel->OnOverlayRender();
        }
    }

    void PanelManager::Serialize() const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Panels" << YAML::Value << YAML::BeginSeq;
        {
            for (const auto& [panelID, panel] : m_Panels)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "ID" << YAML::Value << panelID;
                out << YAML::Key << "Name" << YAML::Value << panel.Name;
                out << YAML::Key << "IsOpen" << YAML::Value << panel.IsOpen;
                out << YAML::EndMap;
            }
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout("EditorLayout.yaml");
        fout << out.c_str();
        fout.close();
    }

    void PanelManager::Deserialize()
    {
        std::string outStr;
        std::filesystem::path layoutPath = "EditorLayout.yaml";
        if (!FileSystem::ReadFile(layoutPath, outStr))
            return;

        YAML::Node data = YAML::Load(outStr);
        if (!data["Panels"])
        {
            // NOTE: Console log
            FOX_ERROR("Failed to load EditorLayout.yaml from % because the file appears to be corrupted!", layoutPath.parent_path().string());
            return;
        }

        for (auto panelNode : data["Panels"])
        {
            PanelData* panelData = GetPanelData(panelNode["ID"].as<uint32_t>(0));

            if (panelData == nullptr)
                continue;

            panelData->IsOpen = panelNode["IsOpen"].as<bool>(panelData->IsOpen);
        }
    }
}
