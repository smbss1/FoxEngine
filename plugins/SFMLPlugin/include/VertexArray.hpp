//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_VERTEXARRAY_HPP
#define FOXENGINE_VERTEXARRAY_HPP

namespace fox
{
    class VertexBuffer;
    class VertexBufferLayout;

    class VertexArray
    {
    public:
        VertexArray();

        ~VertexArray();

        void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

        void Bind() const;

        void Unbind() const;

    private:
        unsigned int m_RendererID{};
    };
}


#endif //FOXENGINE_VERTEXARRAY_HPP
