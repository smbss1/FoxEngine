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

        ImSequencer::Sequencer(m_oSequencer.get(), &m_iCurrentFrame, &m_bExpanded, &m_oSequencer->selected_entry, &m_oSequencer->first_frame, ImSequencer::SEQUENCER_EDIT_ALL | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE);

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
}