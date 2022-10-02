//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_VERTEXARRAY_HPP
#define FOXENGINE_VERTEXARRAY_HPP

namespace fox
{
    class VertexBuffer;
    class IndexBuffer;

    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const ref<IndexBuffer>& indexBuffer) = 0;

        [[nodiscard]] virtual const std::vector<ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        [[nodiscard]] virtual const ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static ref<VertexArray> Create();
    };
}


#endif //FOXENGINE_VERTEXARRAY_HPP
