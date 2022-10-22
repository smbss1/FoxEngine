//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_VERTEXARRAY_HPP
#define FOXENGINE_VERTEXARRAY_HPP

namespace fox
{
    class VertexBuffer;
    class IndexBuffer;

    class VertexArray : public RefCounted
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

        [[nodiscard]] virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        [[nodiscard]] virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static Ref<VertexArray> Create();
    };
}


#endif //FOXENGINE_VERTEXARRAY_HPP
