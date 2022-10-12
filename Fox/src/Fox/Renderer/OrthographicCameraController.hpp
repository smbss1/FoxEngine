//
// Created by samuel on 28/06/2021.
//

#ifndef FOXENGINE_ORTHOGRAPHICCAMERACONTROLLER_HPP
#define FOXENGINE_ORTHOGRAPHICCAMERACONTROLLER_HPP

#include <Events/MouseEvent.hpp>
#include <Events/ApplicationEvent.hpp>
#include "OrthographicCamera.hpp"

namespace fox
{
    class OrthographicCameraController
    {
    public:
        explicit OrthographicCameraController(float aspectRatio, bool rotation = false);

        void OnUpdate(float deltaTime);

        void OnEvent(Event &e);

        void OnResize(float width, float height);

        OrthographicCamera &GetCamera()
        { return m_Camera; }

        [[nodiscard]] const OrthographicCamera &GetCamera() const
        { return m_Camera; }

        [[nodiscard]] float GetZoomLevel() const
        { return m_ZoomLevel; }

        void SetZoomLevel(float level)
        { m_ZoomLevel = level; }

    private:
        bool OnMouseScrolled(MouseScrolledEvent &e);

        bool OnWindowResized(WindowResizeEvent &e);

    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;

        bool m_Rotation;

        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
    };
}

#endif //FOXENGINE_ORTHOGRAPHICCAMERACONTROLLER_HPP
