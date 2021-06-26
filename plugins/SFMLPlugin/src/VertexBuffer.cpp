//
// Created by samuel on 26/06/2021.
//

#include <Renderer.hpp>
#include <Core/Logger/Logger.hpp>
#include "VertexBuffer.hpp"

namespace fox
{
    VertexBuffer::VertexBuffer(const void *data, unsigned int size)
    {
        fox::info("Construct VertexBuffer");

        // Generate a new ID
        GLCall(glGenBuffers(1, &m_RendererID));
        // Init the ID as a array
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        // Fill the array
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }

    VertexBuffer::~VertexBuffer()
    {
        fox::info("Destroy VertexBuffer");
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void VertexBuffer::Bind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }

    void VertexBuffer::Unbind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
}
