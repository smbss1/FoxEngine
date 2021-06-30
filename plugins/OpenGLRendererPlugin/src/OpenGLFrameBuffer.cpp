//
// Created by samuel on 29/06/2021.
//

#include <glad/glad.h>
#include <Core/Assert.hpp>
#include <OpenGLRendererAPI.hpp>
#include "OpenGLFrameBuffer.hpp"

namespace fox
{
    static const uint32_t s_MaxFramebufferSize = 8192;

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification &spec)
        : m_oSpecs(spec)
    {
        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        GLCall(glDeleteFramebuffers(1, &m_RendererID));
        GLCall(glDeleteTextures(1, &m_ColorAttachment));
        GLCall(glDeleteTextures(1, &m_DepthAttachment));
    }

    void OpenGLFrameBuffer::Bind()
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
        glViewport(0, 0, m_oSpecs.Width, m_oSpecs.Height);
    }

    void OpenGLFrameBuffer::Unbind()
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
        {
            fox::warn("Attempted to rezize framebuffer to %, %", width, height);
            return;
        }
        m_oSpecs.Width = width;
        m_oSpecs.Height = height;
        Invalidate();
    }

    void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {

    }

    uint32_t OpenGLFrameBuffer::GetColorAttachmentRendererID(uint32_t index) const
    {
        return m_ColorAttachment;
    }

    const FramebufferSpecification &OpenGLFrameBuffer::GetSpecification() const
    {
        return m_oSpecs;
    }

    int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
        return 0;
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_RendererID)
        {
            GLCall(glDeleteFramebuffers(1, &m_RendererID));
            GLCall(glDeleteTextures(1, &m_ColorAttachment));
            GLCall(glDeleteTextures(1, &m_DepthAttachment));
        }

        GLCall(glCreateFramebuffers(1, &m_RendererID));
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

        GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorAttachment));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_oSpecs.Width, m_oSpecs.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0));

        GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthAttachment));
        GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_oSpecs.Width, m_oSpecs.Height));
//        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_oSpecs.Width, m_oSpecs.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
//                     nullptr));
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0));

        FOX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}