//
// Created by samuel on 27/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "Renderer/GraphicsContext.hpp"
#include "OpenGL/OpenGLContext.hpp"
#include "GlfwWindows/GlfwWindow.hpp"

namespace fox
{
    scope<GraphicsContext> GraphicsContext::Create(void* window)
    {
        return new_scope<OpenGLContext>((GLFWwindow*)window);
    }
}