//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_EDITORCAMERA_HPP
#define FOXENGINE_EDITORCAMERA_HPP

#include <Utils/Vec3.hpp>
#include <Utils/Quat.hpp>
#include <Utils/Vec2.hpp>
#include <Core/Application.hpp>
#include "Camera.hpp"

namespace fox
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;

        void OnUpdate(Application& app);
        virtual void OnZoom(float factor) { };
        virtual void OnMove(const Vec2& offset) { };
        virtual void OnResize(const Vec2& size) { };

        inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }

        inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; OnResize(Vec2(width, height)); }

        const Vec3& GetPosition() const { return m_Position; }
        quat GetOrientation() const;

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }
    private:
        void MousePan(const Vec2& delta);
        void MouseRotate(const Vec2& delta);
        void MouseZoom(float delta);

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;

        void panUp();
        void panDown();
        void panLeft();
        void panRight();

        void zoomOut();
        void setZoom(const int& zoom);

    protected:
        Vec2 m_InitialMousePosition = { 0.0f, 0.0f };

        void zoomIn();

    private:
        Vec3 m_Position = { 0.0f, 0.0f, 0.0f };

        float m_PanningSpeed = 4000.f;

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;
        Vec2 m_Pan;

        int m_Zoom = 0;
        float m_ZoomingRatio = 0.99f;

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
    };
}

#endif //FOXENGINE_EDITORCAMERA_HPP
