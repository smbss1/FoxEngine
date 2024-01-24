//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_CAMERA_HPP
#define FOXENGINE_CAMERA_HPP

#include "glm/glm.hpp"
#include "Core/Logger/Logger.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <Components/Transform.hpp>

namespace fox
{
    class Camera
    {
    public:
        Camera() = default;
        explicit Camera(const glm::mat4& projection, const glm::mat4& view = glm::mat4(1.0f))
                : m_ProjectionMatrix(projection), m_ViewMatrix(view) {}

        virtual ~Camera() = default;

        [[nodiscard]] void SetPosition(const glm::vec3& newPosition, bool recalcute = true)
        {
            m_Position = newPosition;
            if (recalcute) RecalculateViewMatrix();
        }

        [[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }
        [[nodiscard]] float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation, bool recalcute = true)
        {
            m_Rotation = rotation;
            if (recalcute) RecalculateViewMatrix();
        }

        void SetTransform(const TransformComponent& transform)
        {
            m_Position = transform.position;
            m_Rotation = transform.GetRotation().z;
            m_ViewMatrix = transform.GetTransform();
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        [[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        [[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        [[nodiscard]] glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

        inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; OnViewportSizeChanged(); }
        inline void SetViewportPosition(const glm::vec2& position) { m_ViewportPos = position; }
        virtual void OnViewportSizeChanged() {}

        void RecalculateViewMatrix()
        {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

            m_ViewMatrix = glm::inverse(transform);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        glm::vec3 ScreenToWorldSpace(const glm::vec2& screenPos) {

            glm::vec2 viewportMousePos = screenPos - m_ViewportPos;

            //depth = linearizeDepth(depth, m_NearClip, m_FarClip);
            // Convert to NDC
            glm::vec4 ndc(
                (viewportMousePos.x / m_ViewportWidth) * 2.0f - 1.0f,
                1.0f - (viewportMousePos.y / m_ViewportHeight) * 2.0f,
                0.0f, // Adjust based on depth
                1.0f
            );

            //ndc /= m_Distance;

            // Convert NDC to Clip Space
            glm::vec4 clipSpacePos = ndc;

            // Convert Clip Space to Eye (Camera) Space
            glm::vec4 eyeSpacePos = glm::inverse(m_ProjectionMatrix) * clipSpacePos;

            // Convert Eye Space to World Space
            glm::vec4 worldSpacePos = glm::inverse(GetViewMatrix()) * eyeSpacePos;

            if (worldSpacePos.w == 0.0f) {
                // Handle potential division by zero
                return glm::vec3(0.0f);
            }

            return glm::vec3(worldSpacePos) / worldSpacePos.w; // Perspective divide
        }
    protected:
        glm::vec3 m_Position;
        float m_Rotation = 0.0f;

        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;

        glm::vec2 m_ViewportPos;
    };
}

#endif //FOXENGINE_CAMERA_HPP
