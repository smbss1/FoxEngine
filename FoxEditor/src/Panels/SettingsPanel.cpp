//
// Created by samuel on 09/10/22.
//

#include "SettingsPanel.hpp"
#include "imgui.h"
#include "Events/EventSystem.hpp"
#include "EditorEvent.hpp"
#include "Components/Transform.hpp"
#include "Components/Rigidbody2D.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Scripting/ScriptEngine.hpp"

namespace fox
{
    SettingsPanel::SettingsPanel()
    {
        EnableOverlay = true;
        event::EventSystem::Get().On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(SettingsPanel::OnContextChangeChange));
    }

    void SettingsPanel::OnContextChangeChange(const OnContextChangeEvent& event)
    {
        m_pContext = event.Context;
    }

    void SettingsPanel::OnImGui()
    {
        ImGui::Begin("Settings");
        ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
        if (ImGui::Button("Reload C# Assembly"))
        {
            ScriptEngine::ReloadAppDomain();
        }
        ImGui::End();
    }

    void SettingsPanel::OnOverlayRender()
    {
        if (m_ShowPhysicsColliders)
        {
            // Box Colliders
            {
                auto view = m_pContext->GetAllEntitiesWith<TransformComponent, BoxCollider2D>();
                for (auto entity : view)
                {
                    auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2D>(entity);

                    Entity e = Entity(entity, m_pContext.Raw());
                    TransformComponent transformComp = m_pContext->GetWorldSpaceTransform(e);

                    glm::vec3 translation = transformComp.position + glm::vec3(bc2d.Offset, 0.001f);
                    glm::vec3 scale = transformComp.scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                                          * glm::rotate(glm::mat4(1.0f), transformComp.GetRotation().z, glm::vec3(0.0f, 0.0f, 1.0f))
                                          * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
                }
            }

            // Circle Colliders
            {
                auto view = m_pContext->GetAllEntitiesWith<TransformComponent, CircleCollider2D>();
                for (auto entity : view)
                {
                    auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2D>(entity);
                    Entity e = Entity(entity, m_pContext.Raw());
                    TransformComponent transformComp = m_pContext->GetWorldSpaceTransform(e);

                    glm::vec3 translation = transformComp.position + glm::vec3(cc2d.Offset, 0.001f);
                    glm::vec3 scale = transformComp.scale * glm::vec3(cc2d.Radius * 2.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                                          * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
                }
            }
        }
    }
}
