//
// Created by samuel on 28/06/2021.
//

#include "Core/Input/Input.hpp"
#include "Renderer/OrthographicCameraController.hpp"
#include "Core/Logger/Logger.hpp"

namespace fox
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        : m_AspectRatio(aspectRatio)
        , m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
        , m_Rotation(rotation)
    {
    }

    void OrthographicCameraController::OnUpdate(float deltaTime)
    {
        /*if (Input::IsKeyDown(Key::A))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
        }
        else if (Input::IsKeyDown(Key::D))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
        }

        if (Input::IsKeyDown(Key::W))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
        }
        else if (Input::IsKeyDown(Key::S))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
        }

        if (m_Rotation)
        {
            if (Input::IsKeyDown(Key::Q))
                m_CameraRotation += m_CameraRotationSpeed * deltaTime;
            if (Input::IsKeyDown(Key::E))
                m_CameraRotation -= m_CameraRotationSpeed * deltaTime;

            if (m_CameraRotation > 180.0f)
                m_CameraRotation -= 360.0f;
            else if (m_CameraRotation <= -180.0f)
                m_CameraRotation += 360.0f;

            m_Camera.SetRotation(m_CameraRotation);
        }*/

        m_Camera.SetPosition(m_CameraPosition);
        m_CameraTranslationSpeed = m_ZoomLevel;
    }

    void OrthographicCameraController::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(FOX_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        //dispatcher.Dispatch<WindowResizeEvent>(FOX_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
    }

    void OrthographicCameraController::OnResize(const glm::vec2& viewportPosition, float width, float height)
    {
        m_AspectRatio = width / height;
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        m_Camera.SetViewportSize(width, height);
        m_Camera.SetViewportPosition(viewportPosition);
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent &e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent &e)
    {
        //OnResize((float)e.GetWidth(), (float)e.GetHeight());
        return false;
    }
}
