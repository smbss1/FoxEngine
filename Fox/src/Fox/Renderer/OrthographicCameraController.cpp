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
            m_FocalPoint.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
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

        if (Input::IsKeyDown(Key::LeftAlt))
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
            m_InitialMousePosition = mouse;

            if (Input::IsMouseButtonDown(Mouse::ButtonMiddle))
                MousePan(delta);
        }

        UpdateView();
    }

    void OrthographicCameraController::UpdateView()
    {
        // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
        m_CameraPosition = CalculatePosition();
        m_Camera.SetPosition(m_CameraPosition);
    }

    std::pair<float, float> OrthographicCameraController::PanSpeed() const
    {
        float x = std::min(m_Camera.GetViewportSize().x / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_Camera.GetViewportSize().y / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float OrthographicCameraController::ZoomSpeed() const
    {
        float distance = m_ZoomLevel * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

    void OrthographicCameraController::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_ZoomLevel;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_ZoomLevel;
    }

    void OrthographicCameraController::MouseZoom(float delta)
    {
        m_ZoomLevel -= delta * ZoomSpeed();
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    }

    glm::vec3 OrthographicCameraController::GetUpDirection() const
    {
        return glm::vec3(0.0f, 1.0f, 0.0f); // glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 OrthographicCameraController::GetRightDirection() const
    {
        return glm::vec3(1.0f, 0.0f, 0.0f); // glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 OrthographicCameraController::GetForwardDirection() const
    {
        return glm::vec3(0.0f, 0.0f, -1.0f); // glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 OrthographicCameraController::CalculatePosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_ZoomLevel;
    }

    /*glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
    }*/

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
        float delta = e.GetYOffset() * 0.25f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent &e)
    {
        //OnResize((float)e.GetWidth(), (float)e.GetHeight());
        return false;
    }
}
