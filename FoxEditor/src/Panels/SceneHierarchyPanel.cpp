//
// Created by samuel on 25/06/2021.
//

#include "imgui.h"
#include "Core/Application.hpp"
#include "Components.hpp"
#include "Events/EventSystem.hpp"
#include "SceneHierarchyPanel.hpp"
#include "EditorEvent.hpp"
#include "Scene/EntitySerializer.hpp"
#include "Core/Project.hpp"

#include <filesystem>

namespace fox
{
    SceneHierarchyPanel::SceneHierarchyPanel()
    {
        event::EventSystem::Get().On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(SceneHierarchyPanel::OnContextChangeChange));
    }

    SceneHierarchyPanel::~SceneHierarchyPanel()
    {
        m_pContext.Reset();
    }

    void SceneHierarchyPanel::OnContextChangeChange(const OnContextChangeEvent& event)
    {
        m_pContext = event.Context;
        m_SelectedEntity = {};

        event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
    }

    void SceneHierarchyPanel::OnImGui()
    {
        // ImGui::ShowDemoWindow();
        ImGui::Begin("Scene Hierarchy");
        ImGui::BeginChild("###Scene Hierarchy");
        {
            if (m_pContext)
            {
                m_pContext->m_Registry.each([&](auto entityID)
                {
                    Entity entity { entityID, m_pContext.Raw() };

                    if (entity.GetParentUUID() == 0)
                        DrawEntityNode(entity);
                });

                if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
                    m_SelectedEntity = {};
                    event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
                }

                // Right-click on blank space
                if (ImGui::BeginPopupContextWindow(0, 1, false)) {
                    if (ImGui::MenuItem("Create Empty Entity"))
                        m_pContext->NewEntity();

                    ImGui::EndPopup();
                }
            }
        }
        ImGui::EndChild();
        if (ImGui::BeginDragDropTarget() && m_pContext)
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char* path = (const char*)payload->Data;
                auto filepath = Project::AssetsDir() / path;

                if (filepath.extension() == ".foxprefab")
                {
                    Entity entity = EntitySerializer::DeserializeEntityAsPrefab(filepath.c_str(), *m_pContext);
                    event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(entity));
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(const Entity& entity)
    {
        auto &name = entity.get<EntityName>();
        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_OpenOnArrow : 0) | ImGuiTreeNodeFlags_Selected;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

        if (!entity.HasChildren())
            flags |= ImGuiTreeNodeFlags_Leaf;

        bool expanded = ImGui::TreeNodeEx((void*)(uint64_t)entity.GetUUID(), flags, "%s", name.name.c_str());

        // after a click
        if (ImGui::IsItemDeactivated())
        {
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_None))
            {
                m_SelectedEntity = entity;
                event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
            }
        }

        if (ImGui::BeginDragDropSource())
        {
            UUID entityID = entity.GetUUID();
            ImGui::SetDragDropPayload("SceneHierarchy", &entityID, sizeof(UUID));
            ImGui::Text(name.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneHierarchy");

            if (payload)
            {
                size_t count = payload->DataSize / sizeof(UUID);
                for (size_t i = 0; i < count; i++)
                {
                    UUID droppedEntityID = *(((UUID*)payload->Data) + i);
                    Entity droppedEntity = m_pContext->GetEntityByUUID(droppedEntityID);
                    m_pContext->ParentEntity(droppedEntity, entity);
                }
            }

            ImGui::EndDragDropTarget();
        }

        bool bIsDeleted = false; // Is the entity deleted ?
        if (ImGui::BeginPopupContextItem())
        {
            m_SelectedEntity = entity;
            if (ImGui::MenuItem("Delete Entity"))
                bIsDeleted = true;

            ImGui::EndPopup();
        }

//        if (expanded)
//        {
//           // Draw tree child node
//        //    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
//        //    bool xpanded = ImGui::TreeNodeEx((void*)5446, flags, "%s", name.name.c_str());
//        //    if (xpanded)
//        //        ImGui::TreePop();
//            ImGui::TreePop();
//        }

        // Draw children
        //--------------
        if (expanded)
        {
            for (auto child : entity.Children())
                DrawEntityNode(m_pContext->GetEntityByUUID(child));
            ImGui::TreePop();
        }

        if (bIsDeleted && m_SelectedEntity)
        {
            m_pContext->DestroyEntity(m_SelectedEntity);
            if (m_SelectedEntity == entity) {
                m_SelectedEntity = {};
                event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
            }
        }
    }
}
