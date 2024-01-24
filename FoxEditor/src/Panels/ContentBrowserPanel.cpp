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
#include "Asset/AssetManager.hpp"
#include "Asset/TextureImporter.hpp"

#define FOX_CONTENT_BROWSER_ERROR(...) FOX_CORE_ERROR_TAG("Content Browser", __VA_ARGS__)

namespace fox
{
    static float padding = 17.0f; // Padding between folders and files
    static float thumbnailSize = 80.f; // Size of the thumbnail (icon)

    ContentBrowserPanel::ContentBrowserPanel()
        : m_ThumbnailCache(new_ref<ThumbnailCache>())
    {
        m_TreeNodes.push_back(TreeNode(".", 0));

        m_pFolderIcon = TextureImporter::LoadTexture2D("Resources/ContentBrowser/Icons/DirectoryIcon.png");
        m_pFileIcon = TextureImporter::LoadTexture2D("Resources/ContentBrowser/Icons/FileIcon.png");

        RefreshAssetTree();

        m_Mode = Mode::FileSystem;

        event::EventSystem::On<OnProjectChangeEvent>(FOX_BIND_EVENT_FN(ContentBrowserPanel::OnProjectChange));
        event::EventSystem::On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(ContentBrowserPanel::OnContextChange));
    }

    void ContentBrowserPanel::OnProjectChange(const OnProjectChangeEvent& event)
    {
         m_CurrentDirectory = Project::AssetsDir();
         m_BaseDirectory = Project::AssetsDir();
         m_CurrentDirectory = m_BaseDirectory;
    }

    void ContentBrowserPanel::OnContextChange(const OnContextChangeEvent& event)
    {
        m_Context = event.Context;
    }

    void ContentBrowserPanel::OnImGui(bool& isOpen)
    {
        ImGui::Begin(ICON_FA_FOLDER" Content Browser", &isOpen);
        ImGui::BeginChild("###Content Browser");

        if (!m_CurrentDirectory.empty())
        {
            if (m_CurrentDirectory != m_BaseDirectory)
            {
                if (ImGui::Button(ICON_FA_ARROW_LEFT))
                {
                    m_CurrentDirectory = m_CurrentDirectory.parent_path();
                }
            }

            float cellSize = thumbnailSize + padding;

            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int) (panelWidth / cellSize);
            if (columnCount < 1)
                columnCount = 1;

            ImGui::Columns(columnCount, 0, false);

            int id = 0;
            for (auto &directoryEntry : fs::directory_iterator(m_CurrentDirectory))
            {
                const auto &path = directoryEntry.path();
                auto strFilenamePath = path.filename().string();

                int texResourceId = id++;
                ImGui::PushID(texResourceId);

                // THUMBNAIL
                auto relativePath = std::filesystem::relative(path, Project::AssetsDir());
                Ref<Texture2D> thumbnail = m_pFolderIcon;
                if (!directoryEntry.is_directory())
                {
                    thumbnail = m_ThumbnailCache->GetOrCreateThumbnail(relativePath);
                    if (!thumbnail)
                        thumbnail = m_pFileIcon;
                }

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::ImageButton((ImTextureID)thumbnail->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1}, { 1, 0});

                if (ImGui::BeginDragDropSource())
                {
                    auto itemStrPath = relativePath.string();
                    const char* itemPath = itemStrPath.c_str();

                    ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (strlen(itemPath) + 1) * sizeof(char));
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Import"))
                    {
                        AssetManager::ImportAsset(relativePath);
                        RefreshAssetTree();
                    }
                    ImGui::EndPopup();
                }

                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (directoryEntry.is_directory())
                        m_CurrentDirectory /= path.filename();
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

                    fs::path path(m_CurrentDirectory);
                    path /= std::string(droppedEntity.GetName() + ".foxprefab").c_str();
                    EntitySerializer::SerializeEntityAsPrefab(path.string().c_str(), droppedEntity);
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();

        //if (!m_CurrentDirectory.empty())
        //{
        //    if (m_Mode == Mode::Asset)
        //    {
        //        TreeNode* node = &m_TreeNodes[0];

        //        auto currentDir = std::filesystem::relative(m_CurrentDirectory, Project::AssetsDir());
        //        for (const auto& p : currentDir)
        //        {
        //            // if only one level
        //            if (node->Path == currentDir)
        //                break;

        //            if (node->Children.find(p) != node->Children.end())
        //            {
        //                node = &m_TreeNodes[node->Children[p]];
        //                continue;
        //            }
        //            else
        //            {
        //                // can't find path
        //                FOX_CORE_ASSERT(false);
        //            }
        //        }

        //        for (const auto& [item, treeNodeIndex] : node->Children)
        //        {
        //            bool isDirectory = std::filesystem::is_directory(Project::AssetsDir() / item);

        //            std::string itemStr = item.generic_string();

        //            ImGui::PushID(itemStr.c_str());
        //            Ref<Texture2D> icon = isDirectory ? m_pFolderIcon : m_pFileIcon;
        //            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        //            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

        //            if (ImGui::BeginPopupContextItem())
        //            {
        //                if (ImGui::MenuItem("Delete"))
        //                {
        //                    FOX_CORE_ASSERT(false, "Not implemented");
        //                }
        //                ImGui::EndPopup();
        //            }

        //            if (ImGui::BeginDragDropSource())
        //            {
        //                AssetHandle handle = m_TreeNodes[treeNodeIndex].Handle;
        //                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));
        //                ImGui::EndDragDropSource();
        //            }


        //            ImGui::PopStyleColor();
        //            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        //            {
        //                if (isDirectory)
        //                    m_CurrentDirectory /= item.filename();
        //            }

        //            ImGui::TextWrapped(itemStr.c_str());

        //            ImGui::NextColumn();

        //            ImGui::PopID();
        //        }
        //    }
        //    else
        //    {
        //        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        //        {
        //            const auto& path = directoryEntry.path();
        //            std::string filenameString = path.filename().string();

        //            ImGui::PushID(filenameString.c_str());

        //            // THUMBNAIL
        //            auto relativePath = std::filesystem::relative(path, Project::AssetsDir());
        //            Ref<Texture2D> thumbnail = m_pFolderIcon;
        //            if (!directoryEntry.is_directory())
        //            {
        //                thumbnail = m_ThumbnailCache->GetOrCreateThumbnail(relativePath);
        //                if (!thumbnail)
        //                    thumbnail = m_pFileIcon;
        //            }

        //            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        //            ImGui::ImageButton((ImTextureID)thumbnail->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

        //            if (ImGui::BeginPopupContextItem())
        //            {
        //                if (ImGui::MenuItem("Import"))
        //                {
        //                    AssetManager::ImportAsset(relativePath);
        //                    RefreshAssetTree();
        //                }
        //                ImGui::EndPopup();
        //            }

        //            ImGui::PopStyleColor();
        //            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        //            {
        //                if (directoryEntry.is_directory())
        //                    m_CurrentDirectory /= path.filename();
        //            }

        //            ImGui::TextWrapped(filenameString.c_str());

        //            ImGui::NextColumn();

        //            ImGui::PopID();
        //        }
        //    }
        //}
    }

    void ContentBrowserPanel::RefreshAssetTree()
    {
        const auto& assetRegistry = AssetManager::GetAssetRegistry();
        for (const auto& [handle, metadata] : assetRegistry)
        {
            uint32_t currentNodeIndex = 0;

            for (const auto& p : metadata.FilePath)
            {
                auto it = m_TreeNodes[currentNodeIndex].Children.find(p.generic_string());
                if (it != m_TreeNodes[currentNodeIndex].Children.end())
                {
                    currentNodeIndex = it->second;
                }
                else
                {
                    // add node
                    TreeNode newNode(p, handle);
                    newNode.Parent = currentNodeIndex;
                    m_TreeNodes.push_back(newNode);

                    m_TreeNodes[currentNodeIndex].Children[p] = m_TreeNodes.size() - 1;
                    currentNodeIndex = m_TreeNodes.size() - 1;
                }

            }
        }
    }
}

#undef FOX_CONTENT_BROWSER_ERROR(...)
