//
// Created by samuel on 02/07/2021.
//

#include "imgui.h"
#include "Core/Logger/Logger.hpp"
#include "ContentBrowserPanel.hpp"
#include "EditorLayer.hpp"
#include "ImGui/IconsFontAwesome5.hpp"

namespace fox
{
    // Once we have projects, change this
	extern const std::filesystem::path g_AssetPath = "./assets";

    static float padding = 17.0f; // Padding between folders and files
    static float thumbnailSize = 80.f; // Size of the thumbnail (icon)

    ContentBrowserPanel::ContentBrowserPanel()
        : m_oCurrentDirectory(g_AssetPath)
    {
        m_pFolderIcon = Texture2D::Create("Resources/ContentBrowser/Icons/DirectoryIcon.png");
        m_pFileIcon = Texture2D::Create("Resources/ContentBrowser/Icons/FileIcon.png");
    }

    void ContentBrowserPanel::OnImGui()
    {
        ImGui::Begin(ICON_FA_FOLDER" Content Browser");

        if (!m_oCurrentDirectory.empty())
        {
            if (m_oCurrentDirectory != std::filesystem::path(g_AssetPath))
            {
                if (ImGui::Button(ICON_FA_ARROW_LEFT))
                {
                    m_oCurrentDirectory = m_oCurrentDirectory.parent_path();
                }
            }

            // Make sure cached texture icons exist, if they dont remove them from cache
            for (auto it = m_ImageIcons.cbegin(), next_it = it; it != m_ImageIcons.cend(); it = next_it)
            {
                ++next_it;

                if (!std::filesystem::exists((*it).first))
                    m_ImageIcons.erase(it);
            }

            float cellSize = thumbnailSize + padding;

            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int) (panelWidth / cellSize);
            if (columnCount < 1)
                columnCount = 1;

            ImGui::Columns(columnCount, 0, false);

            int id = 0;
            for (auto &directoryEntry : std::filesystem::directory_iterator(m_oCurrentDirectory))
            {
                const auto &path = directoryEntry.path();
                auto strFilenamePath = path.filename().string();

                auto icon = directoryEntry.is_directory() ? m_pFolderIcon : m_pFileIcon;

                int texResourceId = id++;
                ImGui::PushID(texResourceId);

                if (path.extension().string() == ".png")
                {
                    if (m_ImageIcons.find(path.string()) == m_ImageIcons.end())
                        m_ImageIcons[path.string()] = Texture2D::Create(path.string());

                    icon = m_ImageIcons[path.string()];
                }

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1}, { 1, 0});

                if (ImGui::BeginDragDropSource())
                {
                    auto relativePath = std::filesystem::relative(path, g_AssetPath);
                    // auto relativePath = std::filesystem::relative(path, FPaths::AssetsDir());

                    const char* itemPath = relativePath.c_str();

                    ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (strlen(itemPath) + 1) * sizeof(char));
                    ImGui::EndDragDropSource();
                }

                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (directoryEntry.is_directory())
                        m_oCurrentDirectory /= path.filename();
                }

                // Drag for Texture files
                // for (const auto& type : fox::ex::GetSupportedFormats<fox::Texture2D>()) {
                //     // If the extension is not in list continue the for loop
                //     // TODO: Maybe break the loop when the matched extension is found
                //     if (type != path.extension())
                //         continue;
                //     if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                //         std::string texturePath = path.string();
                //         ImGui::SetDragDropPayload(path.extension().string().c_str(), texturePath.data(),
                //                                   sizeof(char) * texturePath.size());
                //         ImGui::Text(texturePath.c_str());
                //         ImGui::EndDragDropSource();
                //     }
                // }

                ImGui::TextWrapped(strFilenamePath.c_str());
                ImGui::NextColumn();
                ImGui::PopID();
            }
            ImGui::Columns(1);

//            ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
//            ImGui::SliderFloat("Padding", &padding, 0, 32);
        }

        ImGui::End();
    }

    void ContentBrowserPanel::OnProjectOpen()
    {
        // m_oCurrentDirectory = FPaths::AssetsDir();
    }
}
