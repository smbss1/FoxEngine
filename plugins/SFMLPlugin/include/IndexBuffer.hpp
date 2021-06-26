//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_INDEXBUFFER_HPP
#define FOXENGINE_INDEXBUFFER_HPP

namespace fox
{
    class IndexBuffer
    {
    public:
        IndexBuffer() = default;
        IndexBuffer(const unsigned int* data, unsigned int count);

        ~IndexBuffer();

        void Bind() const;

        void Unbind() const;

        [[nodiscard]] unsigned int GetCount() const;

    private:
        unsigned int m_RendererID{};
        unsigned int m_Count{};
    };
}

#endif //FOXENGINE_INDEXBUFFER_HPP
