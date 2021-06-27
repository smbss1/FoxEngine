//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_INDEXBUFFER_HPP
#define FOXENGINE_INDEXBUFFER_HPP

#include <Renderer/Buffer.hpp>

namespace fox
{
    class OpenGLIndexBuffer  : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer() = default;
        OpenGLIndexBuffer(const uint32_t* data, unsigned int count);

        ~OpenGLIndexBuffer() override;

        void Bind() const override;

        void Unbind() const override;

        [[nodiscard]] unsigned int GetCount() const override;

    private:
        unsigned int m_RendererID{};
        unsigned int m_Count{};
    };
}

#endif //FOXENGINE_INDEXBUFFER_HPP
