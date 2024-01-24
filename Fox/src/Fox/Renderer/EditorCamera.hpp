//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_EDITORCAMERA_HPP
#define FOXENGINE_EDITORCAMERA_HPP

#include <Events/MouseEvent.hpp>
#include "Camera.hpp"

namespace fox
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate();
        void OnEvent(Event& e);

        inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }

        glm::vec3 GetUpDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3& GetPosition() const { return m_Position; }
        //glm::quat GetOrientation() const;

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }
        
        void OnViewportSizeChanged() override
        {
            UpdateProjection();
        }


        //glm::vec3 ScreenToWorldSpace(const glm::vec2& screenPos, const glm::ivec2& windowSize) {
        //    // Convert to Normalized Device Coordinates
        //    glm::vec4 ndc(
        //        (screenPos.x / windowSize.x) * 2.0f - 1.0f,
        //        1.0f - (screenPos.y / windowSize.y) * 2.0f, // Inverting Y axis
        //        1.0f, // Far plane
        //        1.0f
        //    );

        //    // Convert to clip space
        //    glm::vec4 clipSpacePos = ndc;

        //    // Convert to eye space
        //    glm::vec4 eyeSpacePos = glm::inverse(m_ProjectionMatrix) * clipSpacePos;

        //    // Convert to world space
        //    glm::vec4 worldSpacePos = glm::inverse(GetViewMatrix()) * glm::vec4(eyeSpacePos.x, eyeSpacePos.y, -1.0, 0.0);

        //    return glm::normalize(glm::vec3(worldSpacePos));
        //}

        float linearizeDepth(float depth, float near, float far) {
            float z = depth * 2.0f - 1.0f; // Back to NDC 
            return (2.0f * near * far) / (far + near - z * (far - near));
        }

    private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const;

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;
    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;
    };
}

#endif //FOXENGINE_EDITORCAMERA_HPP
