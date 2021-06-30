//
// Created by samuel on 25/06/2021.
//

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <Components/EntityName.hpp>
#include <Components/Transform.hpp>
#include <Components/CameraComponent.hpp>
#include <Components/SpriteRenderer.hpp>
#include <ImGui/imgui_internal.h>
#include "SceneHierarchyPanel.hpp"

namespace fox
{
    SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& context)
    {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const ref<Scene>& context)
    {
        m_pContext = context;
        m_SelectedEntity = {};
    }

    void SceneHierarchyPanel::OnImGui()
    {
        ImGui::Begin("Scene Hierarchy");
        m_pContext->GetWorld().each([&](auto entityid)
         {
            fox::Entity entity {&m_pContext->GetWorld(), entityid};
             DrawEntityNode(entity);
         });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectedEntity = {};

        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
                m_pContext->NewEntity();

            ImGui::EndPopup();
        }
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
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool expanded = ImGui::TreeNodeEx((void*)entity.get_id(), flags, "%s", name.name.c_str());

        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
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
//            // Draw tree child node
//            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
//            bool expanded = ImGui::TreeNodeEx((void*)5446, flags, "%s", name.name.c_str());
//            if (expanded)
//                ImGui::TreePop();
            ImGui::TreePop();
        }

        if (bIsDeleted) {
            entity.destroy();
            if (m_SelectedEntity == entity)
                m_SelectedEntity = {};
        }
    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0,0});

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        // X
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Y
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Z
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
    }

    void SceneHierarchyPanel::DrawComponents(Entity& entity)
    {
        auto has_name = entity.get<EntityName>();
        if (has_name)
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, has_name->name.c_str());

            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
                has_name->name = buffer;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (ImGui::MenuItem("Camera"))
            {
                m_pContext->AddComponent<CameraComponent>(m_SelectedEntity);
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Sprite Renderer"))
            {
                m_pContext->AddComponent<SpriteRenderer>(m_SelectedEntity);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopItemWidth();

        DrawComponent<fox::TransformComponent>("Transform", entity, [](fox::TransformComponent& transform)
        {
            DrawVec3Control("Position", transform.position);
            glm::vec3 rot = glm::degrees(transform.rotation);
            DrawVec3Control("Rotation", transform.rotation);
            transform.rotation = glm::radians(rot);
            DrawVec3Control("Scale", transform.scale, 1.0f);
        });

        DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cameraComp)
        {
            auto& camera = cameraComp.camera;

            ImGui::Checkbox("Primary", &cameraComp.Primary);

            const char* projectionTypeString[] = { "Perspective", "Orthographic" };
            const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < 2; ++i)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeString[i];
                    if (ImGui::Selectable(projectionTypeString[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeString[i];
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float size = camera.GetOrthographicSize();
                if (ImGui::DragFloat("Size", &size))
                    camera.SetOrthographicSize(size);

                float near = camera.GetOrthographicNearClip();
                if (ImGui::DragFloat("Near", &near))
                    camera.SetOrthographicNearClip(near);

                float far = camera.GetOrthographicFarClip();
                if (ImGui::DragFloat("Far", &far))
                    camera.SetOrthographicFarClip(far);

                ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);
            }
            else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float fov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                if (ImGui::DragFloat("Size", &fov))
                    camera.SetPerspectiveVerticalFOV(glm::radians(fov));

                float near = camera.GetPerspectiveNearClip();
                if (ImGui::DragFloat("Near", &near))
                    camera.SetPerspectiveNearClip(near);

                float far = camera.GetPerspectiveFarClip();
                if (ImGui::DragFloat("Far", &far))
                    camera.SetPerspectiveFarClip(far);
            }
        });

        DrawComponent<SpriteRenderer>("Sprite Renderer", entity,[](SpriteRenderer& renderer)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(renderer.Color));
        });
    }

    template<typename T>
    void SceneHierarchyPanel::DrawComponent(const std::string &name, Entity entity,  std::function<void(T &)> callback)
    {
        const ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
        auto has_component = entity.get<T>();

        if (has_component)
        {
            ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4});
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeFlags, name.c_str());
            ImGui::PopStyleVar();

            ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
            if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool bIsDeleted = false; // Is the component removed ?
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    bIsDeleted = true;
                ImGui::EndPopup();
            }

            if (open)
            {
                callback(*has_component);
                ImGui::TreePop();
            }

            if (bIsDeleted)
            {
                entity.template remove<T>();
            }
        }
    }

}