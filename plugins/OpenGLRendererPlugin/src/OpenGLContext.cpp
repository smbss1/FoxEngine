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
//        void* (*libGetProcAddress)(const char *name) = 0;
//        lib.open("plugins/GLFWPlugin" DL_EXT);
//        lib.sym("libGetProcAddress", libGetProcAddress);
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwMakeContextCurrent(m_WindowHandle);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize Glad!");

        fox::info("OpenGL Info:");
        fox::info("   Vendor: %", glGetString(GL_VENDOR));
        fox::info("   Renderer: %", glGetString(GL_RENDERER));
        fox::info("   Version: %", glGetString(GL_VERSION));

       //  FOX_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 1), "Fox requires at least OpenGL version 4.5!");
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}