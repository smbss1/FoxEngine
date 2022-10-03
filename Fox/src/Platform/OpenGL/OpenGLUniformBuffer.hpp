//
// Created by samuel on 03/10/22.
//

#ifndef FOXENGINE_OPENGLUNIFORMBUFFER_HPP
#define FOXENGINE_OPENGLUNIFORMBUFFER_HPP


#include "Renderer/UniformBuffer.hpp"

namespace fox
{
    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding);
        virtual ~OpenGLUniformBuffer();

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
    private:
        uint32_t m_RendererID = 0;
    };
}


#endif //FOX_LEXER_OPENGLUNIFORMBUFFER_HPP
