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
#include "Physics2D.hpp"
#include "ImGui/ImGuiExtensions.hpp"
#include "Core/Input/Input.hpp"
#include "Core/ProjectSerializer.hpp"

namespace fox
{
    SettingsPanel::SettingsPanel()
    {
        EnableOverlay = true;
        event::EventSystem::On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(SettingsPanel::OnContextChangeChange));
    }

    void SettingsPanel::OnContextChangeChange(const OnContextChangeEvent& event)
    {
        m_pContext = event.Context;
    }

    void SettingsPanel::OnImGui(bool& isOpen)
    {
        m_Project = Project::GetActive();
        if (m_Project == nullptr)
        {
            isOpen = false;
            return;
        }

        {
            ImGui::Begin("Settings", &isOpen);
            defer({ ImGui::End(); });

            if (ImGui::Button("Reload C# Assembly")) {
                ScriptEngine::ReloadAppDomain();
            }
            ImGuiPhysicsSettings();
        }


        if (m_SerializeProject)
        {
            ProjectSerializer serializer(m_Project);
            serializer.Serialize(m_Project->GetConfig().ProjectDirectory + "/" + m_Project->GetConfig().ProjectFileName);
            m_SerializeProject = false;
        }
    }

    void SettingsPanel::ImGuiPhysicsSettings()
    {
        UI::Checkbox("Show physics colliders", m_ShowPhysicsColliders);
        UI::BeginProperties();

        bool renameSelectedLayer = false;
        int32_t layerToDelete = -1;

        if (ImGui::Button("New Layer", { 80.0f, 28.0f }))
        {
            std::string name = "NewLayer_" + Physics2D::GetLayerCount();
            m_SelectedLayer = Physics2D::AddLayer(name);
            renameSelectedLayer = true;
            m_SerializeProject = true;
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 100.0f), ImVec2(9999.0f, 9999.0f));
        ImGui::BeginChild("LayersList");
        {
            defer({ ImGui::EndChild(); });
            for (const auto &layer: Physics2D::GetLayers())
            {
                ImGui::PushID(layer.Name.c_str());

                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
                bool selected = m_SelectedLayer == layer.LayerID;

                if (selected) {
//                    ImGui::PushStyleColor(ImGuiCol_Text, Colours::Theme::backgroundDark);
                    flags |= ImGuiTreeNodeFlags_Selected;
                }

                if (ImGui::TreeNodeEx(layer.Name.c_str(), flags))
                    ImGui::TreePop();

                bool itemClicked =
                    ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) &&
                    ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 1.0f).x == 0.0f &&
                    ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 1.0f).y == 0.0f;

                if (itemClicked)
                    m_SelectedLayer = layer.LayerID;

                if (layer.LayerID > 0 && ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Rename", "F2")) {
                        m_SelectedLayer = layer.LayerID;
                        renameSelectedLayer = true;
                    }

                    if (ImGui::MenuItem("Delete", "Delete")) {
                        if (selected)
                            m_SelectedLayer = -1;

                        layerToDelete = layer.LayerID;
                        m_SerializeProject = true;
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }
            if (Input::IsKeyDown(Key::F2) && m_SelectedLayer != -1)
                renameSelectedLayer = true;

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
                m_SelectedLayer = -1;
        }
        ImGui::NextColumn();

        if (m_SelectedLayer > -1)
        {
            PhysicLayer* selectedLayerInfo;
            Physics2D::GetLayer(m_SelectedLayer, selectedLayerInfo);

            ImVec2 selectedLayerWindowSize = ImVec2(0, 0);
            ImGui::BeginChild("##SelectedLayerWindow", selectedLayerWindowSize, true);
            {
                static char buffer[256];
                strncpy(buffer, selectedLayerInfo->Name.c_str(), 256);
                ImGuiInputTextFlags text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

                if (selectedLayerInfo->Name == "Default")
                    text_flags |= ImGuiInputTextFlags_ReadOnly;

                if (renameSelectedLayer)
                    ImGui::SetKeyboardFocusHere();

                if (UI::InputText("Layer Name:", buffer, 256, text_flags)) {
                    Physics2D::UpdateLayerName(selectedLayerInfo->LayerID, buffer);
                    m_SerializeProject = true;
                }

                UI::BeginProperties();
                for (const auto& layer : Physics2D::GetLayers())
                {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 1.0f); // adding 1px "border"

                    bool shouldCollide = selectedLayerInfo->Collisions & layer.AsBit();
                    if (UI::Property(layer.Name, shouldCollide))
                    {
                        Physics2D::SetCollision(m_SelectedLayer, layer.LayerID, shouldCollide);
                        m_SerializeProject = true;
                    }
                }
                UI::EndProperties();
            }
            ImGui::EndChild();
        }
        if (layerToDelete > 0)
            Physics2D::RemoveLayer(layerToDelete);
        UI::EndProperties();
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
