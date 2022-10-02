//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_IMGUIEXTENSION_HPP
#define FOXENGINE_IMGUIEXTENSION_HPP

#include <string>
#include <glm/vec3.hpp>
#include <Renderer/Texture.hpp>
#include <imgui.h>

namespace fox
{
    namespace imgui
    {
        bool DragVec3(glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);

        void DrawVec3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f,
                             float columnWidth = 100.0f);

        void ImageWithAspect(ref<Texture2D> texture, ImVec2 texture_size, ImVec2 size,
                             const ImVec2 &_uv0 = ImVec2(0, 0), const ImVec2 &_uv1 = ImVec2(1, 1),
                             const ImVec4 &tint_col = ImVec4(1, 1, 1, 1),
                             const ImVec4 &border_col = ImVec4(0, 0, 0, 0));

        void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness);
    }
}

#endif //FOXENGINE_IMGUIEXTENSION_HPP
