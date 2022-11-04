//
// Created by samuel on 27/10/22.
//

#include "AnimatorSpritePanel.hpp"
#include "imgui.h"
#include "ImGui/IconsFontAwesome5.hpp"
#include "ImGui/ImGuiExtensions.hpp"
#include "Components/SpriteRenderer.hpp"
#include "Asset/AssetManager.hpp"
#include "EditorEvent.hpp"

namespace fox
{
    AnimatorSpritePanel::AnimatorSpritePanel()
    {
        event::EventSystem::Get().On<OnSelectedEntityChangeEvent>(FOX_BIND_EVENT_FN(AnimatorSpritePanel::OnSelectedEntityChange));
    }

    void AnimatorSpritePanel::OnSelectedEntityChange(const OnSelectedEntityChangeEvent& event)
    {
        m_Animator = nullptr;
        m_SelectedEntity = event.m_oEntity;
        if (m_SelectedEntity && m_SelectedEntity.has<Animator>())
        {
            m_Animator = &m_SelectedEntity.get<Animator>();
        }
    }

    void AnimatorSpritePanel::OnImGui(bool& isOpen)
    {
//        FOX_PROFILE_SCOPE();

        if (ImGui::Begin(ICON_FA_MALE" Animator", &isOpen))
        {
            if (!m_Animator)
            {
                ImGui::End();
                return;
            }

            if (ImGui::Button("Add"))
            {
                auto animation = new_ref<Animation>();
                animation->Name = "New Animation";
                m_Animator->Animations.push_back(animation);
            }

            static Ref<Animation> selectedAnim;
            static char s_AnimName[255];
            ImGui::BeginChild("Left", ImVec2(160, 0));
            {
                ImGui::BeginChild("Animations", ImVec2(150, 110), true);
                {
                    for (auto animation: m_Animator->Animations) {
                        ImGui::Text(animation->Name.c_str());
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                            selectedAnim = animation;
                            std::strncpy(s_AnimName, selectedAnim->Name.c_str(), 255);
                        }
                    }
                }
                ImGui::EndChild();
                ImGui::InputText("##Animxxx", s_AnimName, 255);
            }
            ImGui::EndChild();

            Ref<Animation> anim = m_Animator->CurrentAnimation;
            if (!anim)
                return;

            ImGui::SameLine();
            ImGui::BeginChild("Right", ImVec2(0, 0), true);
                UI::BeginProperties();
                UI::Property("Speed", anim->Speed);
                UI::EndProperties();

                Ref<Frame> &frame = anim->CurrentFrame;
                if (frame && AssetManager::IsAssetHandleValid(frame->Texture)) {
                    UI::PreviewTexture(frame->Texture, {".png"});
                    ImGui::SameLine();
                } else if (frame) {
                    UI::DropBoxAsset(frame->Texture, {".png"});
                }
                ImGui::BeginChild("##bjkbjfuhhvv", ImVec2(0, 100));
                {
                    if (ImGui::Button(m_Animator->Play ? ICON_FA_STOP : ICON_FA_PLAY))
                    {
                        m_Animator->TogglePlay();
                    }
                    ImGui::SameLine();

                    bool lastLoopState = anim->Loop;
                    if (lastLoopState)
                        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(15, 15, 15, 255));
                    if (ImGui::Button(ICON_FA_SPINNER)) {
                        anim->Loop = !anim->Loop;
                    }
                    if (lastLoopState)
                        ImGui::PopStyleColor();

                    ImGui::SameLine();
                    int max = anim->Frames.empty() ? 0 : anim->Frames.size() - 1;
                    int currentIndex = anim->GetCurrentIndex();
                    if (UI::Slider("##CurrentIndex", currentIndex, 0, max)) {
                        anim->SetCurrentIndex(currentIndex);
                    }
                    if (ImGui::Button("Add Frame")) {
                        Ref<Frame> frame = new_ref<Frame>();
                        anim->Frames.push_back(frame);
                        anim->SetCurrentIndex(anim->Frames.size() - 1);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Remove Frame")) {
                        if (anim->CurrentFrame) {
                            anim->Frames.erase(
                                std::remove(anim->Frames.begin(), anim->Frames.end(), anim->CurrentFrame),
                                anim->Frames.end());
                            anim->SetCurrentIndex(anim->GetCurrentIndex() - 1);
                        }
                    }
                }
                ImGui::EndChild();
            ImGui::EndChild();

            static Ref<Anim::Event> selectedEvent;
            static char s_Name[255];
            ImGui::Text("Events:");
            ImGui::BeginChild("##Events", ImVec2(0, 100), true);
            {
                if (anim->CurrentFrame)
                {
                    auto event = anim->FindEvent(anim->CurrentFrame->EventID);
                    if (event)
                    {
                        ImGui::Text(event->Name.c_str());
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                            selectedEvent = event;
                            std::strncpy(s_Name, selectedEvent->Name.c_str(), 255);
                        }

                        ImGui::SameLine();
                        if (ImGui::Button(ICON_FA_TIMES)) {
                            anim->Events.erase(std::remove(anim->Events.begin(), anim->Events.end(), event),
                                               anim->Events.end());
                        }
                    }
                }
            }
            ImGui::EndChild();

            if (anim->CurrentFrame)
            {
                UI::InputText("Frame:", s_Name, 255);
                if (strlen(s_Name) <= 0)
                    return;
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_PLUS)) {
                    Ref<Anim::Event> event = new_ref<Anim::Event>();
                    event->ID = UUID(Hash::FNVHash(s_Name));
                    event->Name = s_Name;
                    frame->EventID = event->ID;
                    anim->Events.push_back(event);
                    memset(s_Name, 0, 255);
                }
            }
            ImGui::End();
        }
    }
}
