//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_VERTEXBUFFER_HPP
#define FOXENGINE_VERTEXBUFFER_HPP

namespace fox
{
    class VertexBuffer
    {
    public:
        VertexBuffer() = default;
        VertexBuffer(const void *data, unsigned int size);

        ~VertexBuffer();

        void Bind() const;

        void Unbind() const;

    private:
        unsigned int m_RendererID{};
    };
}

#endif //FOXENGINE_VERTEXBUFFER_HPP
