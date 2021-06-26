//
// Created by samuel on 25/06/2021.
//

#include <ImGui/imgui.h>
#include <Components/EntityName.hpp>
#include <Components/Transform.hpp>
#include "SceneHierarchyPanel.hpp"

namespace fox
{
    SceneHierarchyPanel::SceneHierarchyPanel(World* context)
    {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(World* context)
    {
        m_pContext = context;
    }

    void SceneHierarchyPanel::OnImGui()
    {
        ImGui::Begin("Scene Hierarchy");
        m_pContext->each([&](auto entityid)
         {
            fox::Entity entity {m_pContext, entityid};
             DrawEntityNode(entity);
         });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectedEntity = {};
        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectedEntity != Entity())
        {
            DrawComponents(m_SelectedEntity);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity &entity)
    {
        auto &name = entity.get<EntityName>().value();
        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_OpenOnArrow : 0) | ImGuiTreeNodeFlags_Selected;
        bool expanded = ImGui::TreeNodeEx((void*)entity.get_id(), flags, "%s", name.get().c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }

        if (expanded)
        {
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity& entity)
    {
        auto has_name = entity.get<EntityName>();
        if (has_name)
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, has_name->get().c_str());

            if (ImGui::InputText("Name", buffer, sizeof(buffer)))
            {
                has_name->set(buffer);
            }
        }

        auto has_transform = entity.get<fox::Transform>();
        if (has_transform)
        {
            if (ImGui::TreeNodeEx((void*)m_pContext->get_type_idx<fox::Transform>(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {

                float vPos[3] = {has_transform->position.x, has_transform->position.y, has_transform->position.z};
                if (ImGui::DragFloat3("Position", vPos, 0.1f)) {
                    has_transform->position.x = vPos[0];
                    has_transform->position.y = vPos[1];
                    has_transform->position.z = vPos[2];
                }

                float vRot[3] = {has_transform->rotation.x, has_transform->rotation.y, has_transform->rotation.z};
                if (ImGui::DragFloat3("Rotation", vRot, 0.1f)) {
                    has_transform->rotation.x = vRot[0];
                    has_transform->rotation.y = vRot[1];
                    has_transform->rotation.z = vRot[2];
                }

                float vScale[3] = {has_transform->scale.x, has_transform->scale.y, has_transform->scale.z};
                if (ImGui::DragFloat3("Scale", vScale, 0.1f)) {
                    has_transform->scale.x = vScale[0];
                    has_transform->scale.y = vScale[1];
                    has_transform->scale.z = vScale[2];
                }

                ImGui::TreePop();
            }
        }
    }

}