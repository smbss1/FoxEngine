//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_ORTHOGRAPHICCAMERA_HPP
#define FOXENGINE_ORTHOGRAPHICCAMERA_HPP

#include "glm/glm.hpp"

namespace fox
{
    class OrthographicCamera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);

        [[nodiscard]] const glm::vec3& GetPosition() const;
        void SetPosition(const glm::vec3& position);

        [[nodiscard]] float GetRotation() const;
        void SetRotation(float rotation);

        [[nodiscard]] const glm::mat4& GetProjectionMatrix() const;
        [[nodiscard]] const glm::mat4& GetViewMatrix() const;
        [[nodiscard]] const glm::mat4& GetViewProjectionMatrix() const;
    private:
        void RecalculateViewMatrix();


        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;

    };
}

#endif //FOXENGINE_ORTHOGRAPHICCAMERA_HPP
