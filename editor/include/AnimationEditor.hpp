//
// Created by samuel on 15/07/2021.
//

#ifndef FOXENGINE_ANIMATION_EDITOR_HPP
#define FOXENGINE_ANIMATION_EDITOR_HPP

#include <memory>
#include <vector>
#include <map>

#include <imgui.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
    #define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>
#include <ImSequencer.h>
#include <ImCurveEdit.h>
#include <EventSystem/Delegate.hpp>
#include <Logger/Logger.hpp>
#include <Components/Animator/AnimationPlayer.hpp>
#include <Ecs/World.hpp>

#ifdef IMGUI_DEFINE_MATH_OPERATORS
ImVec2 operator*(const ImVec2& lhs, const float rhs);
ImVec2 operator/(const ImVec2& lhs, const float rhs);
ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);
ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs);
ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs);
ImVec2& operator*=(ImVec2& lhs, const float rhs);
ImVec2& operator/=(ImVec2& lhs, const float rhs);
ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs);
ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs);
ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs);
ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs);
ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs);
ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs);
ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs);
#endif

//#define SNAP_T(x) snap(x, 1, 3)

namespace fox
{
    class AnimationEditor
    {
    public:
        AnimationEditor();
        ~AnimationEditor() = default;
        AnimationEditor(AnimationEditor&&) = delete;
        AnimationEditor(const AnimationEditor&) = delete;

        void SetAnimationPlayer(AnimationPlayer* pAnimPlayer)
        {
            m_pAnimationPlayer = pAnimPlayer;
        }

//        void add_line(SequenceItem item) { _sequencer->m_vItems.push_back(item); }
//
//        void remove_line(SequenceItem line) {
//            for (auto begin = _sequencer->m_vItems.begin(); begin != _sequencer->m_vItems.end(); begin++) {
//                if (begin->get() == line.Id) {
//                    _sequencer->m_vItems.erase(begin);
//                    break;
//                }
//            }
//        }

        void OnImGui();
        void OnSelectedEntityChanged(properties::rw_property<Entity>* p);

        bool play() const { return _play; }
        auto current_frame() const { return m_iCurrentFrame; }
        void increment() { m_iCurrentFrame++; }

    private:

        void OnAnimAdded(Timeline& oAnim);
        void OnAnimDeleted(Timeline& oAnim);

        void OnItemAdded();

    private:
        bool _play = false;
        int32_t m_iCurrentFrame = 0;

        bool m_bExpanded = true;

        Timeline* m_pCurrentAnimation = nullptr;
        uint32_t m_iCurrentAnimationIdx = -1;

        Entity m_oSelectedEntity;
        AnimationPlayer* m_pAnimationPlayer = nullptr;
        std::vector<std::string_view> m_vAnimationNames;

        bool m_bOpenPropertyAnimationWindow = false;

        // TODO: Comment
        event::Delegate<void(Timeline&)> OnAnimationAdded;
        event::Delegate<void(Timeline&)> OnAnimationDeleted;
        event::Delegate<void()> OnAnimationPlayEditor;
        event::Delegate<void()> OnAnimationStopEditor;
    };
}

#endif //FOXENGINE_ANIMATION_EDITOR_HPP
