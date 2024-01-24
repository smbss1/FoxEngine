//
// Created by samuel on 27/06/2021.
//

#include "OpenGLRendererAPI.hpp"
#include "OpenGLUtils.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "glad/glad.h"

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

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &pVertexArray, uint32_t uIndexCount)
    {
        pVertexArray->Bind();
        uint32_t count = uIndexCount ? uIndexCount : pVertexArray->GetIndexBuffer()->GetCount();
        GLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
        pVertexArray->Unbind();
    }

    void OpenGLRendererAPI::DrawElementsInstanced(const Ref<VertexArray>& pVertexArray, uint32_t indicesCount, uint32_t instanceCount)
    {
        pVertexArray->Bind();
        GLCall(glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, instanceCount));
        pVertexArray->Unbind();
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        GLCall(glViewport(x, y, width, height));
    }

    void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        vertexArray->Bind();
        GLCall(glDrawArrays(GL_LINES, 0, vertexCount));
        vertexArray->Unbind();
    }

    void OpenGLRendererAPI::SetLineWidth(float width)
    {
        GLCall(glLineWidth(width));
    }
}
