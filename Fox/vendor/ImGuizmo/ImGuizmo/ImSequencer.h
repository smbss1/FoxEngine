#pragma once

#include <cstddef>
#include <imgui.h>
#include <imgui_internal.h>

struct ImDrawList;
struct ImRect;
namespace ImSequencer
{
   enum SEQUENCER_OPTIONS
   {
      SEQUENCER_EDIT_NONE = 0,
      SEQUENCER_EDIT_STARTEND = 1 << 1,
      SEQUENCER_CHANGE_FRAME = 1 << 3,
      SEQUENCER_ADD = 1 << 4,
      SEQUENCER_DEL = 1 << 5,
      SEQUENCER_COPYPASTE = 1 << 6,
      SEQUENCER_EDIT_ALL = SEQUENCER_EDIT_STARTEND | SEQUENCER_CHANGE_FRAME
   };

    struct CustomDraw
    {
        int index;
        ImRect customRect;
        ImRect legendRect;
        ImRect clippingRect;
        ImRect legendClippingRect;

        ImVec2 canvasPos;
        float framePixelWidth;
        int legendWidth;
        int firstFrameUsed;

        // canvas_pos.x + int(i * framePixelWidth) + legendWidth - int(firstFrameUsed * framePixelWidth);
    };

   struct SequenceInterface
   {
      bool focused = false;
      virtual int GetFrameMin() const = 0;
      virtual int GetFrameMax() const = 0;
      virtual int GetItemCount() const = 0;

      virtual void BeginEdit(int /*index*/) {}
      virtual void EndEdit() {}
      virtual int GetItemTypeCount() const { return 0; }
      virtual const char* GetItemTypeName(int /*typeIndex*/) const { return ""; }
      virtual const char* GetItemLabel(int /*index*/) const { return ""; }

      virtual void Get(int index, int** start, int** end, int* type, unsigned int* color) = 0;
      virtual void Add(int /*type*/) {}
      virtual void Del(int /*index*/) {}
      virtual void Duplicate(int /*index*/) {}

      virtual void Copy() {}
      virtual void Paste() {}

      virtual size_t GetCustomHeight(int /*index*/) { return 0; }
      virtual void DoubleClick(int /*index*/) {}
      virtual void CustomDraw(const CustomDraw& /* custom */, ImDrawList* /*draw_list*/) {}
      virtual void CustomDrawCompact(int /*index*/, ImDrawList* /*draw_list*/, const ImRect& /*rc*/, const ImRect& /*clippingRect*/) {}
   };


   // return true if selection is made
   bool Sequencer(SequenceInterface* sequence, int* currentFrame, bool* expanded, int* selectedEntry, int* firstFrame, int sequenceOptions);

}
