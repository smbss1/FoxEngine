//
// Created by samuel on 26/06/2021.
//

#include <Core/Logger/Logger.hpp>
#include <glad/glad.h>
#include <OpenGLRendererAPI.hpp>
#include "VertexBuffer.hpp"

namespace fox
{
    OpenGLVertexBuffer::OpenGLVertexBuffer()
        : m_oLayout()
    {
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
    {
        fox::info("Construct VertexBuffer");

        GLCall(glGenBuffers(1, &m_RendererID));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(const void *data, uint32_t size)
    {
        fox::info("Construct VertexBuffer");

        // Generate a new ID
        GLCall(glGenBuffers(1, &m_RendererID));
        // Init the ID as a array
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        // Fill the array
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        fox::info("Destroy VertexBuffer");
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLVertexBuffer::Bind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }

    void OpenGLVertexBuffer::Unbind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    const BufferLayout &OpenGLVertexBuffer::GetLayout() const
    {
        return m_oLayout;
    }

    void OpenGLVertexBuffer::SetLayout(const BufferLayout &layout)
    {
        m_oLayout = layout;
    }

    void OpenGLVertexBuffer::SetData(const void *data, uint32_t size)
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
    }
}
