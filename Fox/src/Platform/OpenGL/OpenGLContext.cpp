//
// Created by samuel on 27/06/2021.
//

#include <Core/Assert.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "OpenGLContext.hpp"

namespace fox
{
    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle)
        : m_WindowHandle(windowHandle)
    {
        FOX_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);

        // Init glad library using the OpenGL context from glfw
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize Glad!");

        FOX_CORE_INFO("OpenGL Info:");
        FOX_CORE_INFO("   Vendor: %", glGetString(GL_VENDOR));
        FOX_CORE_INFO("   Renderer: %", glGetString(GL_RENDERER));
        FOX_CORE_INFO("   Version: %", glGetString(GL_VERSION));

       FOX_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 1), "Fox requires at least OpenGL version 4.5!");
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}