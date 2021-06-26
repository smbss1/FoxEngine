//
// Created by samuel on 25/06/2021.
//

#include <utility>
#include <Core/Input/InputManager.hpp>
#include "Renderer/EditorCamera.hpp"

namespace fox
{
    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float EditorCamera::RotationSpeed() const
    {
        return 0.8f;
    }

    float EditorCamera::ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

    void EditorCamera::panUp()
    {
        m_Pan.y += m_PanningSpeed;
    }

    void EditorCamera::panDown()
    {
        m_Pan.y -= m_PanningSpeed;
    }

    void EditorCamera::panLeft()
    {
        m_Pan.x += m_PanningSpeed;
    }

    void EditorCamera::panRight()
    {
        m_Pan.x -= m_PanningSpeed;
    }

    void EditorCamera::zoomIn()
    {
        OnZoom(m_ZoomingRatio);
        m_Zoom++;
        (m_PanningSpeed <= 0) ? 1.f : m_PanningSpeed--;
    }

    void EditorCamera::zoomOut()
    {
        OnZoom(1/m_ZoomingRatio);
        m_Zoom--;
        m_PanningSpeed++;
    }

    void EditorCamera::setZoom(const int& zoom)
    {
        if(m_Zoom > zoom)
            OnZoom(1/std::pow(m_ZoomingRatio, (m_Zoom - zoom)));
        if(m_Zoom < zoom)
            OnZoom(pow(m_ZoomingRatio, (zoom - m_Zoom)));

        m_Zoom = zoom;
    }

    void EditorCamera::OnUpdate(Application& app)
    {
        InputManager& inputManager = app.get<InputManager>().value();
        const Vec2& mouse = inputManager.GetMousePosition();
        Vec2 delta = (mouse - m_InitialMousePosition);
        m_InitialMousePosition = mouse;

        if(inputManager.GetKey(Key::Z))
            panUp();
        if(inputManager.GetKey(Key::S))
            panDown();
        if(inputManager.GetKey(Key::Q))
            panLeft();
        if(inputManager.GetKey(Key::D))
            panRight();

        if (inputManager.GetMouseScroll() != Vec2())
            MouseZoom(inputManager.GetMouseScrollDelta());

        OnMove(m_Pan);
        m_Pan = Vec2();
    }

    void EditorCamera::MousePan(const Vec2& delta)
    {
//            if(delta.y > 0.f)
//            {
//                panUp();
//                panUp();
//                panUp();
//            }
//            else
//            {
//                panDown();
//                panDown();
//                panDown();
//            }
//            if(delta.x > 0.f)
//            {
//                panRight();
//                panRight();
//                panRight();
//            }
//            else
//            {
//                panLeft();
//                panLeft();
//                panLeft();
//            }
    }

    void EditorCamera::MouseRotate(const Vec2& delta)
    {
//        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
//        m_Yaw += yawSign * delta.x * RotationSpeed();
//        m_Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta)
    {
        if(delta > 0.f)
        {
            zoomIn();
            zoomIn();
            zoomIn();
        }
        else
        {
            zoomOut();
            zoomOut();
            zoomOut();
        }
    }

    quat EditorCamera::GetOrientation() const
    {
        return quat(-m_Pitch, -m_Yaw, 0.0f, 1);
    }
}