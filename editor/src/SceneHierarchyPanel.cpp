//
// Created by samuel on 25/06/2021.
//

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <Components/EntityName.hpp>
#include <Components/Transform.hpp>
#include <Components/CameraComponent.hpp>
#include <Components/SpriteRenderer.hpp>
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
        bool expanded = ImGui::TreeNodeEx((void*)entity.get_id(), flags, "%s", name.name.c_str());
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
            strcpy(buffer, has_name->name.c_str());

            if (ImGui::InputText("Name", buffer, sizeof(buffer)))
                has_name->name = buffer;
        }

        DrawComponent<fox::TransformComponent>("Transform", [](fox::TransformComponent& transform)
        {
            ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.1f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.1f);
            ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);
        });

        DrawComponent<CameraComponent>("Camera", [](CameraComponent& cameraComp)
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

        DrawComponent<SpriteRenderer>("Sprite Renderer", [](SpriteRenderer& renderer)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(renderer.Color));
        });
    }

    template<typename T>
    void SceneHierarchyPanel::DrawComponent(const std::string &name, std::function<void(T &)> callback)
    {
        auto has_component = m_SelectedEntity.get<T>();
        if (has_component)
        {
            if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, name.c_str())) {
                callback(*has_component);
                ImGui::TreePop();
            }
        }
    }

}