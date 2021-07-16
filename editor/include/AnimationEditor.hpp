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
    struct Frame {
        int64_t index;
    };

//    struct SequencerTrack
//    {
//        std::string name;
//        struct SequencerKey {
//            int32_t start = 0;
//            int32_t end = 100;
//            uint32_t colour = 0x999999FF;
//        };
//        std::vector<SequencerKey> keys{1};
//
//        int32_t frame_count = 0;
//    };

    static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };

    enum class SequenceItemType : int
    {
        Video		= 0,
        Animation	= 1,
        Static3D	= 2,
        //
        Count
    };

    static const std::map<SequenceItemType, std::string> k_SequenceItemType2Str =
            {
                    { SequenceItemType::Video,		"Video" },
                    { SequenceItemType::Animation,	"Animation" },
                    { SequenceItemType::Static3D,	"Static 3D" }
            };


    struct SequenceItem
    {
        template <typename T>
        using delegate = fox::event::Delegate<T>;


        int					Id;
        std::string			Name;
        SequenceItemType	Type;
        int 				FrameStart;
        int 				FrameEnd;
        bool 				Expanded;
        int					Duration;
        delegate<void()>			OnStartPlayback;
        delegate<void()>			OnPause;
        delegate<void()>			OnStop;
        delegate<void()>			OnSeekForward;
        delegate<void()>			OnSeekBackward;
        delegate<void(int)>			OnSeekFrame;
    };

    // It's the number of List of Points, it's not the number of points in the list
    constexpr const uint32_t NumOfElementsList = 1;

    struct RampEdit : public ImCurveEdit::Delegate
    {
        RampEdit()
        {
            m_vPoints[0].push_back(ImVec2(0.f, 0));
            m_vPoints[0].push_back(ImVec2(20.f, 0.6f));
            m_vPoints[0].push_back(ImVec2(25.f, 0.2f));
            m_vPoints[0].push_back(ImVec2(70.f, 0.4f));
            m_vPoints[0].push_back(ImVec2(120.f, 1.f));

            mbVisible[0] = true;
            mMax = ImVec2(1.f, 1.f);
            mMin = ImVec2(0.f, 0.f);
        }

        size_t GetCurveCount()
        {
            return 1;
        }

        bool IsVisible(size_t curveIndex)
        {
            return mbVisible[curveIndex];
        }

        size_t GetPointCount(size_t curveIndex)
        {
            return m_vPoints[curveIndex].size();
        }

        uint32_t GetCurveColor(size_t curveIndex)
        {
            uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
            return cols[curveIndex];
        }

        ImVec2* GetPoints(size_t curveIndex)
        {
            return m_vPoints[curveIndex].data();
        }

        virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveBezier; }

//        inline double snap(double original, int numerator, int denominator)
//        {
//            return round(original * denominator / numerator) * numerator / denominator;
//        }

        virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
        {
            m_vPoints[curveIndex][pointIndex] = ImVec2(value.x, value.y);
            SortValues(curveIndex);
            for (size_t i = 0; i < GetPointCount(curveIndex); i++)
            {
                if (m_vPoints[curveIndex][i].x == value.x)
                    return (int)i;
            }
            return pointIndex;
        }

        virtual void AddPoint(size_t curveIndex, ImVec2 value)
        {
            m_vPoints[curveIndex].push_back(value);
            SortValues(curveIndex);
        }

        virtual ImVec2& GetMax() { return mMax; }
        virtual ImVec2& GetMin() { return mMin; }
        virtual unsigned int GetBackgroundColor() { return 0; }



        std::vector<ImVec2> m_vPoints[NumOfElementsList];
        bool mbVisible[NumOfElementsList];
        ImVec2 mMin;
        ImVec2 mMax;



    private:
        void SortValues(size_t curveIndex)
        {
            auto b = std::begin(m_vPoints[curveIndex]);
            auto e = std::begin(m_vPoints[curveIndex]) + GetPointCount(curveIndex);
            std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

        }
    };

    struct SequencerImpl : public ImSequencer::SequenceInterface
    {
        int GetFrameMin() const override
        {
            return frame_min;
        }

        int GetFrameMax() const override
        {
            return frame_max;
        }

        int GetItemCount() const override
        {
            return m_vItems.size();
        }

        int GetItemTypeCount() const override
        {
            return static_cast<int>(SequenceItemType::Count);
        }

        const SequenceItem& GetItem(int index)
        {
            return m_vItems[index];
        }

        void Get(int index, int** start, int** end, int* type, unsigned int* colour) override
        {
            SequenceItem& item = m_vItems[index];

            if (index < m_vItems.size())
            {
                if (start) {
                    *start = &item.FrameStart;
                }
                if (end) {
                    *end = &item.FrameEnd;
                }
                if (colour) {
                    *colour = 0xFFAA8080;
                }
                if (type) {
                    *type = static_cast<int>(item.Type);
                }
            }
        }

        virtual void Add(int type) override {}

        void Add(const SequenceItem& item)
        {
            m_vItems.push_back(item);
        }

        void Del(int index) override
        {
            m_vItems.erase(m_vItems.begin() + index);
        }

        void Duplicate(int index) override
        {
            return m_vItems.push_back(m_vItems[index]);
        }

        size_t GetCustomHeight(int index) override
        {
            return m_vItems[index].Expanded ? 100 : 0;
        }

        void DoubleClick(int index) override
        {
            if (m_vItems[index].Expanded)
            {
                m_vItems[index].Expanded = false;
                return;
            }
            for (auto& item : m_vItems)
            {
                item.Expanded = false;
            }
            m_vItems[index].Expanded = !m_vItems[index].Expanded;
        }

        const char* GetItemLabel(int index) const override
        {
            if (index < m_vItems.size()) {
                return m_vItems[index].Name.c_str();
            }
            return "";
        }

        template<typename T>
        T map(T x, T in_min, T in_max, T out_min, T out_max)
        {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
        {
            static const char* labels[] = { "Translation", "Rotation" , "Scale" };

            fox::info("Rc: (%, %, %, %)", clippingRect.Min.x, clippingRect.Min.y, clippingRect.Max.x, clippingRect.Max.y);

            rampEdit.mMax = ImVec2(float(frame_max), 1.f);
            rampEdit.mMin = ImVec2(float(frame_min), 0.f);
            draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
            for (int i = 0; i < NumOfElementsList; i++)
            {
                const float spacing = 23.f; // Spacing between labels
                const float offset = 7.f;
                ImVec2 pta(legendRect.Min.x + 30, (legendRect.Min.y + offset) + i * spacing);
                ImVec2 ptb(legendRect.Max.x, (legendRect.Min.y + offset) + (i + 1) * spacing);
                draw_list->AddText(pta, rampEdit.mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
                if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
                    rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];

                ImVec2* points = rampEdit.GetPoints(i);
                size_t count = rampEdit.GetPointCount(i);
                for (size_t k = 0; k < count; ++k)
                {
                    ImVec2& point = points[k];
                    float newvalue = map(pta.y - spacing + offset, legendClippingRect.Max.y, legendClippingRect.Min.y, 0.f, 1.f);

                    point.y = newvalue;
                }
            }
            draw_list->PopClipRect();

            ImGui::SetCursorScreenPos(rc.Min);
            const ImVec2& rect = {rc.Max.x - rc.Min.x, rc.Max.y - rc.Min.y};
            ImCurveEdit::Edit(rampEdit, rect, 137 + index, &clippingRect);
        }

        virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
        {
            rampEdit.mMax = ImVec2(float(frame_max), 1.f);
            rampEdit.mMin = ImVec2(float(frame_min), 0.f);
            draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
            for (int i = 0; i < NumOfElementsList; i++)
            {
                for (int j = 0; j < rampEdit.m_vPoints[i].size(); j++)
                {
                    float p = rampEdit.m_vPoints[i][j].x;
                    if (p < m_vItems[index].FrameStart || p > m_vItems[index].FrameEnd)
                        continue;
                    float r = (p - frame_min) / float(frame_max - frame_min);
                    float x = ImLerp(rc.Min.x, rc.Max.x, r);
                    draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xDDDDDDFF, 4.f);
                }
            }
            draw_list->PopClipRect();
        }

        void BeginEdit(int index) override
        {
            //LOG_DEBUG << "BeginEdit";

        }

        void BeginEdit(int index, int movingPart, int offset) /*override*/
        {
//            auto& item = m_vItems[index];
//            m_CurrentItemEditState.EditMode = movingPart;
//            m_CurrentItemEditState.BeginEditValue = offset;
        }

        void EndEdit() override
        {
        }

        void EndEdit(int index, int movingPart, int offset)
        {
//            m_CurrentItemEditState.EndEditValue = offset;
//            int diff = m_CurrentItemEditState.EndEditValue - m_CurrentItemEditState.BeginEditValue;
//            auto& item = m_vItems[index];
//            if (diff != 0)
//            {
//                if (m_CurrentItemEditState.EditMode == 1)
//                {
//                    LOG_DEBUG << "Expanded/Shrinked from front: " << diff;
//                    item.FrameStart += diff;
//                }
//                else if (m_CurrentItemEditState.EditMode == 2)
//                {
//                    LOG_DEBUG << "Expanded/Shrinked from back: " << diff;
//                    item.FrameEnd += diff;
//                }
//                else if (m_CurrentItemEditState.EditMode == 3)
//                {
//                    LOG_DEBUG << "Moved item: " << diff;
//                    item.SeqFrameStart += diff;
//                    item.SeqFrameEnd += diff;
//                    if (item.SeqFrameEnd > m_FrameMax)
//                    {
//                        m_FrameMax = item.SeqFrameEnd;
//                    }
//                }
//            }
//            //LOG_DEBUG << "Item properties";
//            //LOG_DEBUG << "Sequence frame start: " << item.SeqFrameStart;
//            //LOG_DEBUG << "Sequence frame end  : " << item.SeqFrameEnd;
//            //LOG_DEBUG << "Frame start         : " << item.FrameStart;
//            //LOG_DEBUG << "Frame end           : " << item.FrameEnd;
        }

        std::vector<SequenceItem> m_vItems;

        int selected_entry = 0;
        int first_frame = 0;

        int frame_min = 0;
        int frame_max = 100;
        RampEdit rampEdit;
    };

    class AnimationEditor
    {
    public:
        AnimationEditor() : m_oSequencer(fox::new_scope<SequencerImpl>())
        {
            auto line = SequenceItem();
            line.Name = "sbfdsb";
            m_oSequencer->m_vItems.push_back(line);
        }

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
        fox::scope<SequencerImpl> m_oSequencer = nullptr;
        bool _play = false;
        int32_t m_iCurrentFrame = 0;

        bool m_bExpanded = true;

        Timeline* m_pCurrentAnimation = nullptr;
        uint32_t m_iCurrentAnimationIdx = 0;

        AnimationPlayer* m_pAnimationPlayer = nullptr;
        std::vector<std::string_view> m_vAnimationNames;

        // TODO: Comment
        event::Delegate<void(Timeline&)> OnAnimationAdded;
        event::Delegate<void(Timeline&)> OnAnimationDeleted;
    };
}

#endif //FOXENGINE_ANIMATION_EDITOR_HPP
