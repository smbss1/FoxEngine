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
// #include <rttr/type>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Reflection.hpp"
#include "Utils/Vec3.hpp"
#include "Utils/Quat.hpp"

namespace fox
{
    struct TransformComponent : public ComponentImpl<TransformComponent>
    {
        // REFLECTABLEV(TransformComponent, Component)

    public:
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

        glm::vec3 local_position{};
        glm::vec3 local_rotation{};

        TransformComponent* parent = nullptr;

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
            glm::mat4 rot = glm::toMat4(glm::quat(rotation));
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                                  * rot
                                  * glm::scale(glm::mat4(1.0f), scale);

            if (parent)
                return transform * parent->GetTransform();
            return transform;
        }
    };
}

#endif