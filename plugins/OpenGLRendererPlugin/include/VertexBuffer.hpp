//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_VERTEXBUFFER_HPP
#define FOXENGINE_VERTEXBUFFER_HPP

#include <Renderer/Buffer.hpp>

namespace fox
{
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer();
        OpenGLVertexBuffer(const void *data, unsigned int size);

        ~OpenGLVertexBuffer() override;

        void Bind() const override;

        void Unbind() const override;

        [[nodiscard]] const BufferLayout& GetLayout() const override;
        void SetLayout(const BufferLayout& layout) override;

    private:
        unsigned int m_RendererID{};
        BufferLayout m_oLayout;
    };
}

#endif //FOXENGINE_VERTEXBUFFER_HPP
