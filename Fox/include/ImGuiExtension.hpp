//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_IMGUIEXTENSION_HPP
#define FOXENGINE_IMGUIEXTENSION_HPP

#include <string>
#include <glm/vec3.hpp>

namespace fox
{
    namespace imgui
    {
        bool DragVec3(glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
        void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
    }
}

#endif //FOXENGINE_IMGUIEXTENSION_HPP
