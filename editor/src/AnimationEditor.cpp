//
// Created by samuel on 15/07/2021.
//

#include <cstdint>
#include <algorithm>
#include "AnimationEditor.hpp"

namespace fox
{
    void AnimationEditor::OnImGui()
    {
        auto&& io = ImGui::GetIO();
        ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(10, io.DisplaySize.y - 210), ImGuiCond_Once );
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 10*2, 200), ImGuiCond_Once );
        ImGui::Begin("Animation");

        if (_play) {
            if (ImGui::Button("pause")) {
                _play = false;
            }
        } else {
            if (ImGui::Button("play")) {
                _play = true;
            }
        }

        if (m_pAnimationPlayer)
        {
            if (ImGui::Button("+"))
            {
                Timeline& oNewAnim = m_pAnimationPlayer->AddAnimation("Blank");
                OnAnimationAdded(oNewAnim);

                m_vAnimationNames.push_back(oNewAnim.Name);
            }

            if (ImGui::Button("-") && m_pCurrentAnimation)
            {
                OnAnimationDeleted(*m_pCurrentAnimation);
                m_pAnimationPlayer->RemoveAnimation(m_pCurrentAnimation->Name.get());
            }
        }

        // If we don't have select an entity with an AnimationPlayer, do nothing
        if (m_pAnimationPlayer && !m_pAnimationPlayer->Animations.get().empty())
        {
            std::string_view strPreview = m_vAnimationNames[m_iCurrentAnimationIdx];
            if (ImGui::BeginCombo("Animations", strPreview.data())) {
                for (int i = 0; i < m_vAnimationNames.size(); ++i)
                {
                    bool isSelected = strPreview == m_vAnimationNames[i];
                    if (ImGui::Selectable(m_vAnimationNames[i].data(), isSelected))
                    {
                        m_iCurrentAnimationIdx = i;
                        m_pCurrentAnimation = m_pAnimationPlayer->GetAnimation(m_vAnimationNames[i]);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        } else {
            if (ImGui::BeginCombo("Animations", "[empty]")) {
                ImGui::EndCombo();
            }
        }

        ImSequencer::Sequencer(m_oSequencer.get(), &m_iCurrentFrame, &m_bExpanded, &m_oSequencer->selected_entry, &m_oSequencer->first_frame, ImSequencer::SEQUENCER_EDIT_ALL | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE);

        // TODO: Show list of available animation
        // TODO: Possibility to create animation(s)
        // TODO: Possibility to select animation for usage
        // TODO: Possibility to add track(s)
        // TODO: Possibility to add track(s) bind with a variable
        // TODO: Possibility to add/del keyframe to a track
        // TODO: Possibility to simulate animation in Editor
        // TODO: Snap keys to the grid
//        if (ImGui::BeginDragDropTarget())
//        {
////            Bin::Entry to_add;
////            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("BIN_ENTRY")) {
////                to_add = **(Bin::Entry**)payload->Data;
////                ui.add_node_graph(to_add);
////                graph_changed = true;
////            }
//            ImGui::EndDragDropTarget();
//        }

//        if (current_frame != _current_frame.index) {
//            _current_frame.index = current_frame;
////            for (auto&& line : _sequencer->lines) {
////                line->frame_jumped = true;
////            }
//        }

//        int32_t count = 0;
//        for (auto&& line : _sequencer->m_vItems) {
//            count = std::max(line->, count);
//        }
//        _sequencer->frame_max = count;

        ImGui::End();

    }

    void AnimationEditor::OnSelectedEntityChanged(properties::rw_property<Entity>* p)
    {
        SetAnimationPlayer(nullptr);
        auto pAnimPlayer = p->get().get<AnimationPlayer>();
        if (pAnimPlayer)
            SetAnimationPlayer(pAnimPlayer.get());
    }

}