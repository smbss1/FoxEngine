//
// Created by samuel on 26/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "Renderer/Buffer.hpp"

namespace fox
{
    BufferLayout::BufferLayout() : m_iStride(0)
    {
    }

    BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_vElements(elements)
    {
        CalculateOffsetAndStride();
    }

    unsigned int BufferLayout::GetStride() const
    {
        return m_iStride;
    }

    const std::vector<BufferElement> &BufferLayout::GetElements() const &
    {
        return m_vElements;
    }

    void BufferLayout::CalculateOffsetAndStride()
    {
        uint32_t offset = 0;
        m_iStride = 0;
        for (auto& element : m_vElements)
        {
            element.m_uOffset = offset;
            offset += element.m_uSize;
            m_iStride += element.m_uSize;
        }
    }

    ref<VertexBuffer> VertexBuffer::Create(float *vertices, uint32_t size)
    {
        return RendererAPI::GetGraphicPlugin()->CreateVertexBuffer(vertices, size);
    }

    ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        return RendererAPI::GetGraphicPlugin()->CreateVertexBuffer(size);
    }

    ref<IndexBuffer> IndexBuffer::Create(uint32_t *indices, uint32_t count)
    {
        return RendererAPI::GetGraphicPlugin()->CreateIndexBuffer(indices, count);
    }
}