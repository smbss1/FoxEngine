//
// Created by samuel on 26/06/2021.
//

#include <cassert>
#include <Core/Logger/Logger.hpp>
#include <glad/glad.h>
#include <OpenGLRendererAPI.hpp>
#include "IndexBuffer.hpp"

namespace fox
{
    OpenGLIndexBuffer::OpenGLIndexBuffer(const unsigned int* data, unsigned int count)
        : m_Count(count)
    {
        fox::info("Construct IndexBuffer");

        assert(sizeof(uint32_t) == sizeof(GLuint));
        // Generate a new ID for the indices
        GLCall(glGenBuffers(1, &m_RendererID));
        // Init the ID as a array
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
        // Fill the array
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW));
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        fox::info("Destroy IndexBuffer");
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLIndexBuffer::Bind() const
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    }

    void OpenGLIndexBuffer::Unbind() const
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    unsigned int OpenGLIndexBuffer::GetCount() const
    {
        return m_Count;
    }
}
