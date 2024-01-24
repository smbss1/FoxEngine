//
// Created by samuel on 26/06/2021.
//

#include <cassert>
#include <Core/Assert.hpp>
#include <glad/glad.h>
#include "OpenGLUtils.hpp"
#include "OpenGLVertexArray.hpp"
#include "VertexBuffer.hpp"

namespace fox
{
    GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:     return GL_FLOAT;
            case ShaderDataType::Float2:    return GL_FLOAT;
            case ShaderDataType::Float3:    return GL_FLOAT;
            case ShaderDataType::Float4:    return GL_FLOAT;
            case ShaderDataType::Mat3:      return GL_FLOAT;
            case ShaderDataType::Mat4:      return GL_FLOAT;
            case ShaderDataType::Int:       return GL_INT;
            case ShaderDataType::Int2:      return GL_INT;
            case ShaderDataType::Int3:      return GL_INT;
            case ShaderDataType::Int4:      return GL_INT;
            case ShaderDataType::Bool:      return GL_BOOL;
        }
        // TODO: Error message (Invalid type)
        assert(0);
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        GLCall(glGenVertexArrays(1, &m_RendererID));
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        GLCall(glBindVertexArray(0));
        GLCall(glDeleteVertexArrays(1, &m_RendererID));
    }

    void OpenGLVertexArray::Bind() const
    {
        GLCall(glBindVertexArray(m_RendererID));

        if (m_pIndexBuffer)
            m_pIndexBuffer->Bind();
    }

    void OpenGLVertexArray::Unbind() const
    {
        GLCall(glBindVertexArray(0));

        if (m_pIndexBuffer)
            m_pIndexBuffer->Unbind();

        for (auto& e : m_vVerticesBuffers)
        {
            e->Unbind();
        }
    }

    void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer)
    {
        FOX_CORE_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(), "Vertex Buffer has no layout!");
        FOX_CORE_ASSERT(vertexBuffer->IsUploaded(), "Vertex Buffer hasn't been uploaded!");

        GLCall(glBindVertexArray(m_RendererID));
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : vertexBuffer->GetLayout())
        {
            switch (element.m_eType)
            {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                {
                    glEnableVertexAttribArray(element.m_uIndex);
                    glVertexAttribPointer(element.m_uIndex,
                                          element.GetComponentCount(),
                                          ShaderDataTypeToOpenGLType(element.m_eType),
                                          element.m_bNormalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.m_uOffset);
                    glVertexAttribDivisor(element.m_uIndex, element.m_uDivisor);
                    break;
                }
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                {
                    glEnableVertexAttribArray(element.m_uIndex);
                    glVertexAttribIPointer(element.m_uIndex,
                                           element.GetComponentCount(),
                                           ShaderDataTypeToOpenGLType(element.m_eType),
                                           layout.GetStride(),
                                           (const void*)element.m_uOffset);
                    glVertexAttribDivisor(element.m_uIndex, element.m_uDivisor);
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                {
                   uint8_t count = element.GetComponentCount();
                   for (uint8_t i = 0; i < count; i++)
                    {
                        glEnableVertexAttribArray(element.m_uIndex + i);
                        glVertexAttribPointer(element.m_uIndex + i,
                                              count,
                                              ShaderDataTypeToOpenGLType(element.m_eType),
                                              element.m_bNormalized ? GL_TRUE : GL_FALSE,
                                              layout.GetStride(),
                                              (const void*)(element.m_uOffset + sizeof(float) * count * i));
                        glVertexAttribDivisor(element.m_uIndex + i, element.m_uDivisor);
                    }
                    break;
                }
                default:
                    FOX_CORE_ASSERT(false, "Unknown ShaderDataType!");
            }
        }
        m_vVerticesBuffers.push_back(vertexBuffer);

        GLCall(glBindVertexArray(0));
        vertexBuffer->Unbind();
    }

    void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer)
    {
        m_pIndexBuffer = indexBuffer;
        Bind();
    }

    const std::vector<Ref<VertexBuffer>> &OpenGLVertexArray::GetVertexBuffers() const
    {
        return m_vVerticesBuffers;
    }

    const Ref<IndexBuffer> &OpenGLVertexArray::GetIndexBuffer() const
    {
        return m_pIndexBuffer;
    }
}
