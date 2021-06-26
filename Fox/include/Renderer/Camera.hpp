//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_CAMERA_HPP
#define FOXENGINE_CAMERA_HPP

namespace fox
{
    class Camera
    {
    public:
        Camera() = default;
//        Camera(const glm::mat4& projection)
//                : m_Projection(projection) {}

        virtual ~Camera() = default;

//        const glm::mat4& GetProjection() const { return m_Projection; }
    protected:
//        glm::mat4 m_Projection = glm::mat4(1.0f);
    };
}

#endif //FOXENGINE_CAMERA_HPP
