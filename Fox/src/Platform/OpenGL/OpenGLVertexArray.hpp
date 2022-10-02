//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_OPENGL_VERTEXARRAY_HPP
#define FOXENGINE_OPENGL_VERTEXARRAY_HPP

#include <vector>
#include <common.hpp>
#include "Renderer/VertexArray.hpp"

namespace fox
{
    class VertexBuffer;
    class VertexBufferLayout;

    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();

        ~OpenGLVertexArray() override;

        void AddVertexBuffer(const ref<VertexBuffer>& vertexBuffer) override;
        void SetIndexBuffer(const ref<IndexBuffer>& indexBuffer) override;
        [[nodiscard]] const std::vector<ref<VertexBuffer>>& GetVertexBuffers() const override;
        [[nodiscard]] const ref<IndexBuffer>& GetIndexBuffer() const override;

        void Bind() const override;

        void Unbind() const override;

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_uVertexBufferIndex = 0;
        std::vector<ref<VertexBuffer>> m_vVerticesBuffers;
        ref<IndexBuffer> m_pIndexBuffer;
    };
}


#endif //FOXENGINE_VERTEXARRAY_HPP
