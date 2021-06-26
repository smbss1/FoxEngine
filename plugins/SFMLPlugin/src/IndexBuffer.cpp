//
// Created by samuel on 26/06/2021.
//

#include <Renderer.hpp>
#include <cassert>
#include <Core/Logger/Logger.hpp>
#include "IndexBuffer.hpp"

namespace fox
{
    IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
        : m_Count(count)
    {
        fox::info("Construct IndexBuffer");

        assert(sizeof(unsigned int) == sizeof(GLuint));
        // Generate a new ID for the indices
        GLCall(glGenBuffers(1, &m_RendererID));
        // Init the ID as a array
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
        // Fill the array
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
    }

    IndexBuffer::~IndexBuffer()
    {
        fox::info("Destroy IndexBuffer");
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void IndexBuffer::Bind() const
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    }

    void IndexBuffer::Unbind() const
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    unsigned int IndexBuffer::GetCount() const
    {
        return m_Count;
    }
}
