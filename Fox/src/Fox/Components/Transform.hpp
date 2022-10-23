/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Transform.hpp
*/

#ifndef FOX_TRANSFORM_HPP_
#define FOX_TRANSFORM_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Utils/Vec3.hpp"
#include "Utils/Quat.hpp"

#include "Core/UUID.hpp"
#include "Math/Math.hpp"

namespace fox
{
    struct TransformComponent
    {
    private:
        glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
        glm::quat rotationQuat = { 1.0f, 0.0f, 0.0f, 0.0f };

    public:
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

        /**
        * @brief Contructor
        */
        TransformComponent() = default;

        /**
         * @brief Contructor
         * @param pos the position of the object
         */
        explicit TransformComponent(const glm::vec3& pos) : position(pos) {}

        /**
         * @brief Contructor
         * @param pos the position of the object
         * @param sc the scale of the object
         */
        TransformComponent(const glm::vec3& pos, const glm::vec3& sc) : position(pos), scale(sc) {}

        /**
         * @brief Contructor
         * @param pos the position of the object
         * @param rot the rotation of the object
         * @param sc the scale of the object
         */
        TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sc) : position(pos), rotation(rot), scale(sc) {}

        glm::mat4 GetTransform() const
        {
            glm::mat4 rot = glm::toMat4(rotationQuat);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                                  * rot
                                  * glm::scale(glm::mat4(1.0f), scale);
            return transform;
        }

        void SetTransform(const glm::mat4& transform)
        {
            DecomposeTransform(transform, position, rotation, scale);
            rotationQuat = glm::quat(rotation);
//            rotation = glm::eulerAngles(rotationQuat);
        }

//        void SetRotationEuler(const glm::vec3& euler)
//        {
//            rotationEuler = euler;
//            rotation = glm::quat(rotationEuler);
//        }

        void SetRotation(const glm::vec3& euler)
        {
            rotation = euler;
            rotationQuat = glm::quat(rotation);
        }

        void SetRotationQuat(const glm::quat& quat)
        {
            rotationQuat = quat;
            rotation = glm::eulerAngles(rotationQuat);
        }

        glm::vec3 GetRotation() const
        {
            return rotation;
        }
    };

    struct HierarchyComponent
    {
        UUID ParentID = 0;
        std::vector<UUID> Children;

        HierarchyComponent() = default;
        HierarchyComponent(const HierarchyComponent& other) = default;
    };
}

#endif
