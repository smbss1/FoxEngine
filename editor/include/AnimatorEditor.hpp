//
// Created by samuel on 10/07/2021.
//

#ifndef FOXENGINE_ANIMATOREDITOR_HPP
#define FOXENGINE_ANIMATOREDITOR_HPP

#include <string>
#include <vector>

#include <imgui_node_editor.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace ed = ax::NodeEditor;

class Animator;
struct OnSelectedEntityChangeEvent;

namespace fox
{
    class AnimatorEditor
    {
        struct EAnimation;

        struct Pin
        {
            ed::PinId   ID;
            AnimatorEditor::EAnimation*     Node;
            std::string Name;
            // PinType     Type;
            ed::PinKind     Kind;

            Pin(int id, const char* name):
                    ID(id), Node(nullptr), Name(name), Kind(ed::PinKind::Input)
            {
            }
        };

        struct EAnimation
        {
            ed::NodeId ID;
            std::string Name;
            std::vector<Pin> Inputs;
            std::vector<Pin> Outputs;
            ImColor Color;
//            NodeType Type;
            ImVec2 Size;

            std::string State;
            std::string SavedState;

            EAnimation(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
                    ID(id), Name(name), Color(color), /*Type(NodeType::Blueprint),*/ Size(0, 0)
            {
            }
        };

        // Struct to hold basic information about connection between
        // pins. Note that connection (aka. link) has its own ID.
        // This is useful later with dealing with selections, deletion
        // or other operations.
        struct Link
        {
            ed::LinkId ID;

            ed::PinId StartPinID;
            ed::PinId EndPinID;

            ImColor Color;

            Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId):
                    ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
            {
            }
        };


    public:
        AnimatorEditor();
        explicit AnimatorEditor(Animator& animator);
        ~AnimatorEditor();
        void OnImGui();

        void SetContext(Entity& entity);
        void SetContext(Animator& animator);

        void OnSelectedEntityChange(OnSelectedEntityChangeEvent e);

    private:

        EAnimation * NewAnimation(const std::string& strName);

        Pin* FindPin(ed::PinId id);
        EAnimation* FindNode(ed::NodeId id);

        static void ImGuiEx_BeginColumn()
        {
            ImGui::BeginGroup();
        }

        static void ImGuiEx_NextColumn()
        {
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
        }

        static void ImGuiEx_EndColumn()
        {
            ImGui::EndGroup();
        }

        static void BuildNode(EAnimation* node)
        {
            for (auto& input : node->Inputs)
            {
                input.Node = node;
                input.Kind = ed::PinKind::Input;
            }

            for (auto& output : node->Outputs)
            {
                output.Node = node;
                output.Kind = ed::PinKind::Output;
            }
        }

        void BuildNodes();

        ed::EditorContext *m_pNodeEditorContext = nullptr;

        ImVector<Link>   m_vLinks;                  // List of live links. It is dynamic unless you want to create read-only view over nodes.
        int                  m_NextLinkId = 100;        // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.
        std::vector<EAnimation>    m_vAnims;
        std::vector<Link>    m_Links;
        int m_iNextStateId = 1;

        Animator* m_pAnimator = nullptr;

        ed::NodeId m_oContextNodeId = 0;
        ed::LinkId m_oContextLinkId = 0;
        ed::PinId m_oContextPinId = 0;
    };
}

#endif //FOXENGINE_ANIMATOREDITOR_HPP
