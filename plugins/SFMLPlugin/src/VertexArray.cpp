//
// Created by samuel on 26/06/2021.
//

#include <Renderer.hpp>
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

namespace fox
{
    VertexArray::VertexArray()
    {
        GLCall(glGenVertexArrays(1, &m_RendererID));
    }

    VertexArray::~VertexArray()
    {
        GLCall(glDeleteVertexArrays(1, &m_RendererID));
    }

    void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
    {
        Bind();
        vb.Bind();
        const auto& elements = layout.GetElements();
        unsigned int offset = 0;

        for (unsigned int i = 0; i < elements.size(); ++i)
        {
            const auto& element = elements[i];
            GLCall(glEnableVertexAttribArray(i));
            GLCall(glVertexAttribPointer(i, element.m_iCount, element.m_iType,
                             element.m_bNormalized, layout.GetStride(), (const void*)offset));
            offset += element.m_iCount * VertexBufferElement::GetSizeOfType(element.m_iType);
        }
    }

    void VertexArray::Bind() const
    {
        GLCall(glBindVertexArray(m_RendererID));
    }

    void VertexArray::Unbind() const
    {
        GLCall(glBindVertexArray(0));
    }
}