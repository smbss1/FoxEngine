//
// Created by samuel on 02/07/2021.
//

#include <ImGui/imgui.h>
#include <FPaths.hpp>
#include <Core/Logger/Logger.hpp>
#include "ContentBrowserPanel.hpp"

namespace fox
{
    ContentBrowserPanel::ContentBrowserPanel()
        : m_oCurrentDirectory(FPaths::AssetsDir())
    {
        m_pFolderIcon = Texture2D::Create("Resources/ContentBrowser/Icons/DirectoryIcon.png");
        m_pFileIcon = Texture2D::Create("Resources/ContentBrowser/Icons/FileIcon.png");
    }

    void ContentBrowserPanel::OnImGui()
    {
        ImGui::Begin("Content Browser");

        if (!m_oCurrentDirectory.empty()) {

            if (m_oCurrentDirectory != std::filesystem::path(FPaths::AssetsDir())) {
                if (ImGui::Button("<")) {
                    m_oCurrentDirectory = m_oCurrentDirectory.parent_path();
                }
            }

            static float padding = 16.0f;
            static float thumbnailSize = 90.f;
            float cellSize = thumbnailSize + padding;

            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int) (panelWidth / cellSize);
            if (columnCount < 1)
                columnCount = 1;
            ImGui::Columns(columnCount, 0, false);

            int id = 0;
            for (auto &directoryEntry : std::filesystem::directory_iterator(m_oCurrentDirectory)) {
                const auto &path = directoryEntry.path();
                auto relativePath = std::filesystem::relative(path, FPaths::AssetsDir());
                auto strFilenamePath = relativePath.filename().string();

                auto icon = directoryEntry.is_directory() ? m_pFolderIcon : m_pFileIcon;

                int texResourceId = id++;
                ImGui::PushID(texResourceId);
                ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1}, { 1, 0});
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (directoryEntry.is_directory())
                        m_oCurrentDirectory /= path.filename();
                }

                // Drag
                /*
                 * TODO: Instead of doing this if for every file image format, it could be better to save a list in a json file or in a static temporary char*
                 */
                if (path.extension() == ".png")
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        std::string texturePath = path.string();
                        ImGui::SetDragDropPayload("TEXTURE_HANDLE_ID", texturePath.data(), sizeof(char) * texturePath.size());
                        ImGui::Text(texturePath.c_str());
                        ImGui::EndDragDropSource();
                    }

                ImGui::TextWrapped(strFilenamePath.c_str());

                ImGui::PopID();
                ImGui::NextColumn();
            }
            ImGui::Columns(1);

            ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
            ImGui::SliderFloat("Padding", &padding, 0, 32);
        }

        ImGui::End();
    }

    void ContentBrowserPanel::OnProjectOpen()
    {
        m_oCurrentDirectory = FPaths::AssetsDir();
    }
}