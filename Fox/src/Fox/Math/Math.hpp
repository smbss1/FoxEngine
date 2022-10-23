//
// Created by samuel on 01/07/2021.
//

#ifndef FOXENGINE_MATH_HPP
#define FOXENGINE_MATH_HPP

#include <glm/glm.hpp>

namespace fox
{
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale);
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
}

#endif //FOXENGINE_MATH_HPP
