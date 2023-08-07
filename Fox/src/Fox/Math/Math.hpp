//
// Created by samuel on 01/07/2021.
//

#ifndef FOXENGINE_MATH_HPP
#define FOXENGINE_MATH_HPP

#include <glm/glm.hpp>

namespace fox
{
    namespace math
    {
        struct vec3
        {
            glm::vec3 vec;

            vec3() : vec() {}
            vec3(const glm::vec3& v) : vec(v) {}

            operator glm::vec3() const { return vec; }
            operator const glm::vec3& () const { return vec; }

            void SetX(float x) { vec.x = x; }
            void SetY(float x) { vec.y = x; }
            void SetZ(float x) { vec.z = x; }
//            void Set(const std::array<float, 3>& array)
//            {
//                vec.x = array[0];
//                vec.y = array[1];
//                vec.z = array[2];
//            }

            float GetX() const { return vec.x; }
            float GetY() const { return vec.y; }
            float GetZ() const { return vec.z; }

//            const std::array<float, 3>& Get() const { return {vec.x, vec.y, vec.z}; }
        };
    }
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale);
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
}

#endif //FOXENGINE_MATH_HPP
