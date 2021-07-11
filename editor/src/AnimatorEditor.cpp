//
// Created by samuel on 10/07/2021.
//

#include <Components/Animator/Animator.hpp>
#include <Ecs/World.hpp>
#include <Events/EventSystem.hpp>
#include <Core/Base.hpp>
#include <EditorEvent.hpp>
#include "AnimatorEditor.hpp"

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

namespace fox
{
    AnimatorEditor::AnimatorEditor() : m_pAnimator(nullptr)
    {
        // Initialize Node Editor Context
        ed::Config config;
        config.SettingsFile = "Simple.json";
        m_pNodeEditorContext = ed::CreateEditor(&config);

        event::EventSystem::Get().On<OnSelectedEntityChangeEvent>(FOX_BIND_EVENT_FN(AnimatorEditor::OnSelectedEntityChange));
    }

    AnimatorEditor::AnimatorEditor(Animator& animator) : AnimatorEditor()
    {
        SetContext(animator);
    }

    void AnimatorEditor::OnSelectedEntityChange(OnSelectedEntityChangeEvent e)
    {
        m_pAnimator = nullptr;
        if (e.m_oEntity)
            SetContext(e.m_oEntity);
    }

    void AnimatorEditor::SetContext(Entity& entity)
    {
        auto has_animator = entity.get<Animator>();
        if (has_animator)
            SetContext(*has_animator);
    }

    void AnimatorEditor::SetContext(Animator& animator)
    {
        m_vAnims.clear();
        m_pAnimator = &animator;

        for (auto& stringToId : animator.GetStringToNodeId())
        {
            auto& anim = animator.GetGraph().node(stringToId.second);
            EAnimation animation(stringToId.second, anim->Name.get().c_str());
            m_vAnims.push_back(animation);
        }
    }

    AnimatorEditor::~AnimatorEditor()
    {
        // Destroy Node Editor Context
        ed::DestroyEditor(m_pNodeEditorContext);
    }

    AnimatorEditor::Pin* AnimatorEditor::FindPin(ed::PinId id)
    {
        if (!id)
            return nullptr;

        for (auto& node : m_vAnims)
        {
            for (auto& pin : node.Inputs)
                if (pin.ID == id)
                    return &pin;

            for (auto& pin : node.Outputs)
                if (pin.ID == id)
                    return &pin;
        }

        return nullptr;
    }

    void AnimatorEditor::OnImGui()
    {
        static bool createNewNode  = false;
        static Pin* newNodeLinkPin = nullptr;
        static Pin* newLinkPin     = nullptr;

//        auto showLabel = [](const char* label, ImColor color)
//        {
//            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
//            auto size = ImGui::CalcTextSize(label);
//
//            auto padding = ImGui::GetStyle().FramePadding;
//            auto spacing = ImGui::GetStyle().ItemSpacing;
//
//            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));
//
//            auto rectMin = ImGui::GetCursorScreenPos() - padding;
//            auto rectMax = ImGui::GetCursorScreenPos() + size + padding;
//
//            auto drawList = ImGui::GetWindowDrawList();
//            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
//            ImGui::TextUnformatted(label);
//        };

        ImGui::Begin("Animator");
        {
            if (m_pAnimator) {
                ed::SetCurrentEditor(m_pNodeEditorContext);
                ed::Begin("My Editor", ImVec2(0.0, 0.0f));
                {
                    for (auto &node : m_vAnims)
                    {
                        ed::BeginNode(node.ID);
                        char buffer[256];
                        std::strncpy(buffer, node.Name.c_str(), sizeof(buffer));
                        if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                            m_pAnimator->Get(node.Name)->Name = buffer;
                            node.Name = buffer;
                        }

                        for (auto &input_pin : node.Inputs) {
                            ed::BeginPin(input_pin.ID, ed::PinKind::Input);
                            ImGui::Text(input_pin.Name.c_str());
                            ed::EndPin();
                        }
                        ImGui::SameLine();
                        for (auto &output_pin : node.Outputs) {
                            ed::BeginPin(output_pin.ID, ed::PinKind::Output);
                            ImGui::Text(output_pin.Name.c_str());
                            ed::EndPin();
                        }

                        ed::EndNode();
                    }

                    // Submit Links
                    for (auto &link : m_vLinks)
                        ed::Link(link.ID, link.StartPinID, link.EndPinID);

                    //
                    // 2) Handle interactions
                    //

                    // Handle creation action, returns true if editor want to create new object (node or link)
                    if (ed::BeginCreate())
                    {
                        ed::PinId inputPinId, outputPinId;
                        if (ed::QueryNewLink(&inputPinId, &outputPinId))
                        {
                            // QueryNewLink returns true if editor want to create new link between pins.
                            //
                            // Link can be created only for two valid pins, it is up to you to
                            // validate if connection make sense. Editor is happy to make any.
                            //
                            // Link always goes from input to output. User may choose to drag
                            // link from output pin or input pin. This determine which pin ids
                            // are valid and which are not:
                            //   * input valid, output invalid - user started to drag new ling from input pin
                            //   * input invalid, output valid - user started to drag new ling from output pin
                            //   * input valid, output valid   - user dragged link over other pin, can be validated

                            if (inputPinId && outputPinId) // both are valid, let's accept link
                            {
                                if (inputPinId == outputPinId)
                                    ed::RejectNewItem();
                            }
                            // ed::AcceptNewItem() return true when user release mouse button.
                            else
                            {
                                if (ed::AcceptNewItem()) {
                                    // Since we accepted new link, lets add one to our list of links.
                                    m_vLinks.push_back(Link(ed::LinkId(m_NextLinkId++), inputPinId, outputPinId));

                                    // Draw new link.
                                    ed::Link(m_vLinks.back().ID, m_vLinks.back().StartPinID, m_vLinks.back().EndPinID);
                                }
                            }

                            // You may choose to reject connection between these nodes
                            // by calling ed::RejectNewItem(). This will allow editor to give
                            // visual feedback by changing link thickness and color.
                        }
                    }

//                        ed::PinId pinId = 0;
//                        if (ed::QueryNewNode(&pinId)) {
//                            newLinkPin = FindPin(pinId);
//                            if (newLinkPin)
//                                showLabel("+ Create Node", ImColor(32, 45, 32, 180));
//
//                            if (ed::AcceptNewItem()) {
//                                createNewNode = true;
//                                newNodeLinkPin = FindPin(pinId);
//                                newLinkPin = nullptr;
//                                ed::Suspend();
//                                ImGui::OpenPopup("Create New Node");
//                                ed::Resume();
//                            }
//                        }
//                    }
                    ed::EndCreate(); // Wraps up object creation action handling.


                    // Handle deletion action
                    if (ed::BeginDelete()) {
                        // There may be many links marked for deletion, let's loop over them.
                        ed::LinkId deletedLinkId;
                        while (ed::QueryDeletedLink(&deletedLinkId)) {
                            // If you agree that link can be deleted, accept deletion.
                            if (ed::AcceptDeletedItem()) {
                                // Then remove link from your data.
                                for (auto &link : m_vLinks) {
                                    if (link.ID == deletedLinkId) {
                                        m_vLinks.erase(&link);
                                        break;
                                    }
                                }
                            }

                            // You may reject link deletion by calling:
                            // ed::RejectDeletedItem();
                        }
                    }
                    ed::EndDelete(); // Wrap up deletion action

                    auto openPopupPosition = ImGui::GetMousePos();
                    ed::Suspend();
                    if (ed::ShowNodeContextMenu(&m_oContextNodeId))
                        ImGui::OpenPopup("Node Context Menu");
                    else if (ed::ShowPinContextMenu(&m_oContextPinId))
                        ImGui::OpenPopup("Pin Context Menu");
                    else if (ed::ShowLinkContextMenu(&m_oContextLinkId))
                        ImGui::OpenPopup("Link Context Menu");
                    else if (ed::ShowBackgroundContextMenu()) {
                        ImGui::OpenPopup("Create New Node");
                        newNodeLinkPin = nullptr;
                    }
                    ed::Resume();

                    ed::Suspend(); // Suspend the interaction of the node editor, to get the possibility of clicking in InGuiItemsPopup

                    if (ImGui::BeginPopup("Node Context Menu"))
                    {
                        if (ImGui::MenuItem("Delete Animation"))
                        {
                            auto node = FindNode(m_oContextNodeId);
                            m_pAnimator->RemoveAnim(node->Name);
                            ed::DeleteNode(m_oContextNodeId);
                        }
                        ImGui::EndPopup();
                    }

                    if (ImGui::BeginPopup("Create New Node")) {
                        auto newNodePostion = openPopupPosition;
                        EAnimation *node = nullptr;
                        //                if (ImGui::MenuItem("Input Action"))
                        //                    node = SpawnInputActionNode();
                        //                if (ImGui::MenuItem("Output Action"))
                        //                    node = SpawnOutputActionNode();
                        //                if (ImGui::MenuItem("Branch"))
                        //                    node = SpawnBranchNode();
                        //                if (ImGui::MenuItem("Do N"))
                        //                    node = SpawnDoNNode();
                        //                if (ImGui::MenuItem("Set Timer"))
                        //                    node = SpawnSetTimerNode();
                        //                if (ImGui::MenuItem("Less"))
                        //                    node = SpawnLessNode();
                        //                if (ImGui::MenuItem("Weird"))
                        //                    node = SpawnWeirdNode();
                        //                if (ImGui::MenuItem("Trace by Channel"))
                        //                    node = SpawnTraceByChannelNode();
                        //                if (ImGui::MenuItem("Print String"))
                        //                    node = SpawnPrintStringNode();
                        //                ImGui::Separator();
                        //                if (ImGui::MenuItem("Comment"))
                        //                    node = SpawnComment();
                        //                ImGui::Separator();
                        //                if (ImGui::MenuItem("Sequence"))
                        //                    node = SpawnTreeSequenceNode();
                        //                if (ImGui::MenuItem("Move To"))
                        //                    node = SpawnTreeTaskNode();
                        //                if (ImGui::MenuItem("Random Wait"))
                        //                    node = SpawnTreeTask2Node();
                        //                ImGui::Separator();
                        //                if (ImGui::MenuItem("Message"))
                        //                    node = SpawnMessageNode();
                        //                ImGui::Separator();
                        //                if (ImGui::MenuItem("Transform"))
                        //                    node = SpawnHoudiniTransformNode();
                        if (ImGui::MenuItem("New Empty Animation"))
                            node = NewAnimation("Blank");

                        if (node) {
                            BuildNodes();

                            createNewNode = false;

                            ed::SetNodePosition(node->ID, newNodePostion);

                            //                    if (auto startPin = newNodeLinkPin)
                            //                    {
                            //                        auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;
                            //
                            //                        for (auto& pin : pins)
                            //                        {
                            //                            if (CanCreateLink(startPin, &pin))
                            //                            {
                            //                                auto endPin = &pin;
                            //                                if (startPin->Kind == PinKind::Input)
                            //                                    std::swap(startPin, endPin);
                            //
                            //                                s_Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                            //                                s_Links.back().Color = GetIconColor(startPin->Type);
                            //
                            //                                break;
                            //                            }
                            //                        }
                            //                    }
                        }

                        ImGui::EndPopup();
                    } else
                        createNewNode = false;
                }
                ed::Resume(); // Resume the interaction of the node editor
                // End of interaction with editor.
                ed::End();
                ed::SetCurrentEditor(nullptr);
            }
        }
        ImGui::End();
    }

    AnimatorEditor::EAnimation* AnimatorEditor::NewAnimation(const std::string& strName)
    {
        EAnimation anim(m_iNextStateId++, strName.c_str());
        anim.Inputs.push_back(Pin(m_iNextStateId++, "In"));
        anim.Outputs.push_back(Pin(m_iNextStateId++, "Out"));
        m_vAnims.push_back(anim);
        BuildNode(&m_vAnims.back());
        m_pAnimator->add_anim(strName);
        return &m_vAnims.back();
    }

    void AnimatorEditor::BuildNodes()
    {
        for (auto& node : m_vAnims)
            BuildNode(&node);
    }

    AnimatorEditor::EAnimation *AnimatorEditor::FindNode(ed::NodeId id)
    {
        for (auto& node : m_vAnims)
            if (node.ID == id)
                return &node;
        return nullptr;
    }
}