//
// Created by samuel on 02/07/2021.
//

#include "imgui.h"
#include "Core/Logger/Logger.hpp"
#include "ContentBrowserPanel.hpp"
#include "EditorLayer.hpp"
#include "ImGui/IconsFontAwesome5.hpp"
#include "Scene/EntitySerializer.hpp"
#include "EditorEvent.hpp"

#define FOX_CONTENT_BROWSER_ERROR(...) FOX_CORE_ERROR_TAG("Content Browser", __VA_ARGS__)

namespace fox
{
    static float padding = 17.0f; // Padding between folders and files
    static float thumbnailSize = 80.f; // Size of the thumbnail (icon)

    ContentBrowserPanel::ContentBrowserPanel()
    {
        m_pFolderIcon = Texture2D::Create("Resources/ContentBrowser/Icons/DirectoryIcon.png");
        m_pFileIcon = Texture2D::Create("Resources/ContentBrowser/Icons/FileIcon.png");

        event::EventSystem::On<OnProjectChangeEvent>(FOX_BIND_EVENT_FN(ContentBrowserPanel::OnProjectChange));
        event::EventSystem::On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(ContentBrowserPanel::OnContextChange));
    }

    void ContentBrowserPanel::OnProjectChange(const OnProjectChangeEvent& event)
    {
         m_oCurrentDirectory = Project::AssetsDir();
    }

    void ContentBrowserPanel::OnContextChange(const OnContextChangeEvent& event)
    {
        m_Context = event.Context;
    }

    void ContentBrowserPanel::OnImGui(bool& isOpen)
    {
        ImGui::Begin(ICON_FA_FOLDER" Content Browser", &isOpen);
        ImGui::BeginChild("###Content Browser");

        if (!m_oCurrentDirectory.empty())
        {
            if (m_oCurrentDirectory != Project::AssetsDir())
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
                    auto relativePath = std::filesystem::relative(path, Project::AssetsDir());
                    const char* itemPath = relativePath.c_str();

                    ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (strlen(itemPath) + 1) * sizeof(char));
                    ImGui::EndDragDropSource();
                }

                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (directoryEntry.is_directory())
                        m_oCurrentDirectory /= path.filename();
                    if (path.extension().string() == ".foxscene") {
                        EditorLayer::Get().OpenScene(path);
                    }
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
        }

        ImGui::EndChild();

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SceneHierarchy"))
            {
                size_t count = payload->DataSize / sizeof(UUID);

                for (size_t i = 0; i < count; i++)
                {
                    UUID droppedEntityID = *(((UUID*)payload->Data) + i);
                    Entity droppedEntity = m_Context->GetEntityByUUID(droppedEntityID);

                    if (!droppedEntity)
                    {
                        FOX_CONTENT_BROWSER_ERROR("Failed to find Entity with ID % in current Scene context!", droppedEntityID);
                        continue;
                    }

                    std::filesystem::path path(m_oCurrentDirectory);
                    path /= std::string(droppedEntity.GetName() + ".foxprefab").c_str();
                    EntitySerializer::SerializeEntityAsPrefab(path.string().c_str(), droppedEntity);
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }
}

#undef FOX_CONTENT_BROWSER_ERROR(...)
