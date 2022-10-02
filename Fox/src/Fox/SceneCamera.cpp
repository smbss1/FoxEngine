//
// Created by samuel on 29/06/2021.
//

#include <glm/gtc/matrix_transform.hpp>
#include <Core/Assert.hpp>
#include "SceneCamera.hpp"

namespace fox
{

    SceneCamera::SceneCamera()
    {
        RecalculateProjection();
    }

    void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;
        m_PerspectiveFOV = verticalFOV;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;
        RecalculateProjection();
    }

    void SceneCamera::SetOrthographic(float size, float near, float far)
    {
        m_ProjectionType = ProjectionType::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = near;
        m_OrthographicFar = far;
        RecalculateProjection();
    }

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        FOX_CORE_ASSERT(width > 0 && height > 0, "");
        m_AspectRatio = (float) width / (float) height;
        RecalculateProjection();
    }

    void SceneCamera::RecalculateProjection()
    {
        if (m_ProjectionType == ProjectionType::Perspective)
        {
            m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        }
        else
        {
            float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoTop = m_OrthographicSize * 0.5f;

            m_Projection = glm::ortho(orthoLeft, orthoRight,
                                      orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
        }
    }
}