//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_OPENGLCONTEXT_HPP
#define FOXENGINE_OPENGLCONTEXT_HPP

#include "Renderer/GraphicsContext.hpp"

struct GLFWwindow;

namespace fox
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        explicit OpenGLContext(GLFWwindow* windowHandle);
        ~OpenGLContext() override = default;

        void Init() override;
        void SwapBuffers() override;
    private:
        GLFWwindow* m_WindowHandle;
    };
}

#endif //FOXENGINE_OPENGLCONTEXT_HPP
