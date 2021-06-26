//
// Created by samuel on 26/06/2021.
//

#include "VertexBufferLayout.hpp"

namespace fox
{
    VertexBufferLayout::VertexBufferLayout() : m_iStride(0)
    {
    }

    unsigned int VertexBufferLayout::GetStride() const
    {
        return m_iStride;
    }

    const std::vector<VertexBufferElement> &VertexBufferLayout::GetElements() const &
    {
        return m_vElements;
    }
}