//
// Created by samuel on 15/07/2021.
//

#include <cstdint>
#include <algorithm>
#include <Inspector/inspectors.hpp>
#include <Scene/SceneSerializer.hpp>
#include <Components/Transform.hpp>
#include "AnimationEditor.hpp"

// TODO: Possibility to add track(s)
// TODO: Possibility to add track(s) bind with a variable
// TODO: Possibility to add/del keyframe to a track
// TODO: Possibility to simulate animation in Editor
// TODO: Snap keys to the grid

namespace fox
{
    AnimationEditor::AnimationEditor()
    {
        // Bind to self Events
        OnAnimationAdded += event::MakeFunc(*this, &AnimationEditor::OnAnimAdded);
        OnAnimationDeleted += event::MakeFunc(*this, &AnimationEditor::OnAnimDeleted);
    }

    void AnimationEditor::OnImGui()
    {
        auto&& io = ImGui::GetIO();
        ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(10, io.DisplaySize.y - 210), ImGuiCond_Once );
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 10*2, 200), ImGuiCond_Once );
        ImGui::Begin("Animation");
        {
            if (_play) {
                if (ImGui::Button("pause")) {
                    _play = false;
                    OnAnimationPlayEditor();
                }
            } else {
                if (ImGui::Button("play")) {
                    OnAnimationStopEditor();
                    _play = true;
                }
            }

            ImGui::SameLine(0, 20);

            if (ImGui::Button("..."))
                ImGui::OpenPopup("ANIMATION_MENU");

            // Popup for Animation Menu
            if (ImGui::BeginPopup("ANIMATION_MENU")) {
                if (ImGui::MenuItem("New")) {
                    if (m_pAnimationPlayer) {
                        Timeline &oNewAnim = m_pAnimationPlayer->AddAnimation("Blank");
                        OnAnimationAdded(oNewAnim);
                    }
                }

                if (ImGui::MenuItem("Delete")) {
                    if (m_pAnimationPlayer && m_pCurrentAnimation) {
                        OnAnimationDeleted(*m_pCurrentAnimation);
                        m_pAnimationPlayer->RemoveAnimation(m_pCurrentAnimation->Name.get());
                    }
                }

                // ImGuiItemFlags_SelectableDontClosePopup is needed when using MenuItem with Popups
                // otherwise the popups will not open
                ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
                static char strBufferRename[256]; // Temporary buffer for renaming animation when needed
                if (ImGui::MenuItem("Rename")) {
                    if (m_pAnimationPlayer && m_pCurrentAnimation) {
                        strncpy(strBufferRename, m_pCurrentAnimation->Name.get().data(), 256);
                        ImGui::OpenPopup("ANIMATION_RENAME");
                    }
                }
                ImGui::PopItemFlag();

                // Popup for renaming the animation
                if (ImGui::BeginPopupModal("ANIMATION_RENAME"))
                {
                    bool apply;

                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("New Name:");
                    ImGui::SameLine();

                    ImGui::PushItemWidth(100);
                    apply = ImGui::InputText("##EditorContentBrowser", strBufferRename, 256,
                                             ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::PopItemWidth();
                    ImGui::SetItemDefaultFocus(); // Unfortunately this doesn't activate the InputText as desirable, and SetKeyboardFocusHere() is currently a mess.

                    ImGui::Separator();

                    apply |= ImGui::Button("Apply", ImVec2(120, 0));

                    // Need to apply modifiactions ?
                    if (apply) {
                        m_pCurrentAnimation->Name = strBufferRename;
                        m_vAnimationNames[m_iCurrentAnimationIdx] = m_pCurrentAnimation->Name.get();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();

                    // Cancel the modification
                    if (ImGui::Button("Cancel", ImVec2(120, 0)))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::SameLine(0, 20);

            // If we have select an entity with an AnimationPlayer, draw animations combo
            if (m_pAnimationPlayer)
            {
                std::string_view strPreview = m_pCurrentAnimation ? m_vAnimationNames[m_iCurrentAnimationIdx]
                                                                  : "[empty]";
                if (ImGui::BeginCombo("Animations", strPreview.data()))
                {
                    for (int i = 0; i < m_vAnimationNames.size(); ++i) {
                        bool isSelected = strPreview == m_vAnimationNames[i];
                        if (ImGui::Selectable(m_vAnimationNames[i].data(), isSelected)) {
                            m_iCurrentAnimationIdx = i;
                            m_pCurrentAnimation = m_pAnimationPlayer->GetAnimation(m_vAnimationNames[i]);
                        }
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
            else
            {
                if (ImGui::BeginCombo("Animations", "[empty]"))
                    ImGui::EndCombo();
            }
        }

        if (ImGui::Button("Add Track") && m_pCurrentAnimation)
        {
            m_bOpenPropertyAnimationWindow = true;
        }

        float fMaxTimes = 0;
        float fTime = 0;
        if (m_pCurrentAnimation)
        {
            int i = 0;
            for (auto& track : m_pCurrentAnimation->Tracks)
            {
                ImGui::PushID(i);
                {
                    ImGui::Text(("Track " + std::to_string(i)).c_str());
                    ImGui::Indent();

                    rttr::variant track_var = track.get();
                    inspect_ui_var(track_var);

                    ImGui::Unindent();
                }
                ImGui::PopID();

                fMaxTimes = std::max(fMaxTimes, track->GetEndTime());

                i++;
            }
            fTime = m_pCurrentAnimation->Time.get();
        }

        if (ImGui::SliderFloat("Frames", &fTime, 0, fMaxTimes))
        {
            if (m_pCurrentAnimation)
                m_pCurrentAnimation->Time = fTime;
        }

        if (m_bOpenPropertyAnimationWindow)
        {
            if (ImGui::Begin("Add Property", &m_bOpenPropertyAnimationWindow, ImGuiWindowFlags_NoCollapse))
            {
                auto vComponents = m_oSelectedEntity.GetAll();
                for (auto& component_ptr : vComponents)
                {
                    auto component_type = rttr::type::get(*component_ptr);
                    auto properties = component_type.get_properties();
                    for (auto& prop : properties)
                    {
                        if (ImGui::Selectable(prop.get_name().data()))
                        {
                            rttr::instance object = *component_ptr;
                            rttr::variant prop_var = prop.get_value(object);
                            inspect_track_var(prop_var);

                            ITrack* track = prop_var.get_value<ITrack*>();
                            if (track) {
                                track->SetObject(object);
                                track->SetProperty(prop);
                                m_pCurrentAnimation->add(track);
                            } else {
                                fox::error("Invalid Track<%> because is NULL", prop.get_type().get_name());
                            }

                            m_bOpenPropertyAnimationWindow = false;
                        }
                    }
                }
                ImGui::End();
            }
        }
        ImGui::End();
    }

    void AnimationEditor::OnSelectedEntityChanged(properties::rw_property<Entity>* p)
    {
        SetAnimationPlayer(nullptr);
        auto pAnimPlayer = p->get().get<AnimationPlayer>();
        if (pAnimPlayer) {
            m_oSelectedEntity = p->get();
            SetAnimationPlayer(pAnimPlayer.get());
        }
    }

    void AnimationEditor::OnAnimAdded(Timeline& oAnim)
    {
        m_vAnimationNames.push_back(oAnim.Name.get());
    }

    void AnimationEditor::OnAnimDeleted(Timeline& oAnim)
    {
        m_pCurrentAnimation = nullptr;
        m_iCurrentAnimationIdx = -1;
        m_vAnimationNames.erase(std::find(m_vAnimationNames.begin(), m_vAnimationNames.end(), oAnim.Name.get()));
    }

    void AnimationEditor::OnItemAdded()
    {
        m_bOpenPropertyAnimationWindow = true;
    }
}