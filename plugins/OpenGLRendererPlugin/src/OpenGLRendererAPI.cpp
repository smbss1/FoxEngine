//
// Created by samuel on 27/06/2021.
//

#include "OpenGLRendererAPI.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Buffer.hpp"
#include "glad/glad.h"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char *function, const char *file, int line)
{
    bool res = true;
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        std::cerr << "[OpenGL] " << error << " | " << function << " " << file << " (" << line << ")" << std::endl;
        res = false;
    }
    return res;
}

namespace fox
{
    void OpenGLRendererAPI::Init()
    {
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_DEPTH_TEST));
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color)
    {
        GLCall(glClearColor(color.r, color.g, color.b, color.a));
    }

    void OpenGLRendererAPI::Clear()
    {
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    void OpenGLRendererAPI::DrawIndexed(const ref<VertexArray> &pVertexArray, uint32_t uIndexCount)
    {
        uint32_t uCount = uIndexCount ? uIndexCount : pVertexArray->GetIndexBuffer()->GetCount();
        GLCall(glDrawElements(GL_TRIANGLES, uCount, GL_UNSIGNED_INT, nullptr));
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        GLCall(glViewport(x, y, width, height));
    }
}