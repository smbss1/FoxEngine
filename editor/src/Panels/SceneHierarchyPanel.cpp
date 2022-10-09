//
// Created by samuel on 25/06/2021.
//

#include "imgui.h"
#include "Core/Application.hpp"
#include "Components.hpp"
#include "Save for later/FPaths.hpp"
#include "Events/EventSystem.hpp"
#include "SceneHierarchyPanel.hpp"
#include "EditorEvent.hpp"

#include <filesystem>

namespace fox
{
    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel()
    {
        event::EventSystem::Get().On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(SceneHierarchyPanel::OnContextChangeChange));
    }

    SceneHierarchyPanel::~SceneHierarchyPanel()
    {
        m_pContext.reset();
    }

    void SceneHierarchyPanel::OnContextChangeChange(const OnContextChangeEvent& event)
    {
        m_pContext = event.Context;
        m_SelectedEntity = {};

        event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
    }

    void SceneHierarchyPanel::OnImGui()
    {
        ImGui::Begin("Scene Hierarchy");
        {
            if (m_pContext)
            {
                m_pContext->m_Registry.each([&](auto entityID)
                {
                    Entity entity {entityID, m_pContext.get()};
                    DrawEntityNode(entity);
                });

                if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
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
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity &entity)
    {
        auto &name = entity.get<EntityName>();
        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_OpenOnArrow : 0) | ImGuiTreeNodeFlags_Selected;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

        bool expanded = ImGui::TreeNodeEx((void*)(uint64_t)entity.GetUUID(), flags, "%s", name.name.c_str());

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("SceneHierarchy", &entity, sizeof(Entity));
            ImGui::Text(name.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !m_SelectedEntity)
        {
            m_SelectedEntity = entity;
            event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
        }

        bool bIsDeleted = false; // Is the entity deleted ?
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
                bIsDeleted = true;

            ImGui::EndPopup();
        }

        if (expanded)
        {
           // Draw tree child node
        //    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        //    bool xpanded = ImGui::TreeNodeEx((void*)5446, flags, "%s", name.name.c_str());
        //    if (xpanded)
        //        ImGui::TreePop();
            ImGui::TreePop();
        }

//        if (ImGui::BeginDragDropTarget())
//        {
//            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneHierarchy");
//            if (payload != nullptr) {
//                Entity *e = static_cast<Entity *>(payload->Data);
//                fox::info("Entity name: %", e->get<EntityName>().name);
//            }
//            ImGui::EndDragDropTarget();
//        }

        if (bIsDeleted) {
            m_pContext->DestroyEntity(m_SelectedEntity);
            if (m_SelectedEntity == entity) {
                m_SelectedEntity = {};
                event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
            }
        }
    }
}
