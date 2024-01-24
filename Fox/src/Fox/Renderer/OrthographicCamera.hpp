//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_ORTHOGRAPHICCAMERA_HPP
#define FOXENGINE_ORTHOGRAPHICCAMERA_HPP

#include "glm/glm.hpp"
#include "Renderer/Camera.hpp"

namespace fox
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);

    private:

    };
}

#endif //FOXENGINE_ORTHOGRAPHICCAMERA_HPP
