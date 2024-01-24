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

        void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
        void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
        [[nodiscard]] const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override;
        [[nodiscard]] const Ref<IndexBuffer>& GetIndexBuffer() const override;

        void Bind() const override;
        void Unbind() const override;

    private:
        bool m_UseInstancing = false;
        uint32_t m_RendererID = 0;
        std::vector<Ref<VertexBuffer>> m_vVerticesBuffers;
        Ref<IndexBuffer> m_pIndexBuffer;
    };
}


#endif //FOXENGINE_VERTEXARRAY_HPP
