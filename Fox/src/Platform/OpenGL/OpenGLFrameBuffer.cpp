//
// Created by samuel on 29/06/2021.
//

#include <glad/glad.h>
#include <Core/Assert.hpp>
#include "OpenGLFrameBuffer.hpp"
#include "OpenGLUtils.hpp"

namespace fox
{
    static const uint32_t s_MaxFramebufferSize = 8192;
    namespace Utils {

        static GLenum TextureTarget(bool multisampled)
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
        {
            GLCall(glCreateTextures(TextureTarget(multisampled), count, outID));
        }

        static void BindTexture(bool multisampled, uint32_t id)
        {
            GLCall(glBindTexture(TextureTarget(multisampled), id));
        }

        static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE));
            }
            else
            {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr));

                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            }

            GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0));
        }

        static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE));
            }
            else
            {
                GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height));

                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            }

            GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0));
        }

        static bool IsDepthFormat(FramebufferTextureFormat format)
        {
            switch (format)
            {
                case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
            }

            return false;
        }

        static GLenum FoxFBTextureFormatToGL(FramebufferTextureFormat format)
        {
            switch (format)
            {
                case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
                case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
            }

            FOX_CORE_ASSERT(false, "");
            return 0;
        }

    }

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification &spec)
        : m_oSpecs(spec)
    {
        for (auto spec : m_oSpecs.Attachments.Attachments)
        {
            if (!Utils::IsDepthFormat(spec.TextureFormat))
                m_vColorAttachmentSpecs.emplace_back(spec);
            else
                m_oDepthAttachmentSpec = spec;
        }
        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        GLCall(glDeleteFramebuffers(1, &m_RendererID));
        GLCall(glDeleteTextures(m_vColorAttachments.size(), m_vColorAttachments.data()));
        GLCall(glDeleteTextures(1, &m_uDepthAttachment));
    }

    void OpenGLFrameBuffer::Bind()
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
        GLCall(glViewport(0, 0, m_oSpecs.Width, m_oSpecs.Height));
    }

    void OpenGLFrameBuffer::Unbind()
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
        {
            FOX_CORE_WARN("Attempted to rezize framebuffer to %, %", width, height);
            return;
        }
        m_oSpecs.Width = width;
        m_oSpecs.Height = height;
        Invalidate();
    }

    void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {
        FOX_CORE_ASSERT(attachmentIndex < m_vColorAttachments.size(), "Invalid Color Attachment index");

        auto& spec = m_vColorAttachmentSpecs[attachmentIndex];
        glClearTexImage(m_vColorAttachments[attachmentIndex], 0,
                        Utils::FoxFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
    }

    uint32_t OpenGLFrameBuffer::GetColorAttachmentRendererID(uint32_t index) const
    {
        FOX_CORE_ASSERT(index < m_vColorAttachments.size(), "Invalid Color Attachments index");
        return m_vColorAttachments[index];
    }

    const FramebufferSpecification &OpenGLFrameBuffer::GetSpecification() const
    {
        return m_oSpecs;
    }

    int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
        FOX_CORE_ASSERT(attachmentIndex < m_vColorAttachments.size(), "Invalid Color Attachment index");

        GLCall(glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex));
        int pixelData;
        GLCall(glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData));
        return pixelData;
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_RendererID)
        {
            GLCall(glDeleteFramebuffers(1, &m_RendererID));
            GLCall(glDeleteTextures(m_vColorAttachments.size(), m_vColorAttachments.data()));
            GLCall(glDeleteTextures(1, &m_uDepthAttachment));

            m_vColorAttachments.clear();
            m_uDepthAttachment = 0;
        }
        // Create an Id as a framebuffer type
        GLCall(glCreateFramebuffers(1, &m_RendererID));
        // Select this framebuffer
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

        // Attachements
        bool multisample = m_oSpecs.Samples > 1;
        if (!m_vColorAttachmentSpecs.empty())
        {
            m_vColorAttachments.resize(m_vColorAttachmentSpecs.size());
            Utils::CreateTextures(multisample, m_vColorAttachments.data(), m_vColorAttachments.size());

            for (size_t i = 0; i < m_vColorAttachments.size(); i++)
            {
                Utils::BindTexture(multisample, m_vColorAttachments[i]);
                switch (m_vColorAttachmentSpecs[i].TextureFormat)
                {
                    case FramebufferTextureFormat::RGBA8:
                        Utils::AttachColorTexture(m_vColorAttachments[i], m_oSpecs.Samples, GL_RGBA8, GL_RGBA, m_oSpecs.Width, m_oSpecs.Height, i);
                        break;
                    case FramebufferTextureFormat::RED_INTEGER:
                        Utils::AttachColorTexture(m_vColorAttachments[i], m_oSpecs.Samples, GL_R32I, GL_RED_INTEGER, m_oSpecs.Width, m_oSpecs.Height, i);
                        break;
                    default:
                        break;
                }
            }
        }

        if (m_oDepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
        {
            Utils::CreateTextures(multisample, &m_uDepthAttachment, 1);
            Utils::BindTexture(multisample, m_uDepthAttachment);
            switch (m_oDepthAttachmentSpec.TextureFormat)
            {
                case FramebufferTextureFormat::DEPTH24STENCIL8:
                    Utils::AttachDepthTexture(m_uDepthAttachment, m_oSpecs.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_oSpecs.Width, m_oSpecs.Height);
                    break;
                default:
                    break;
            }
        }

        if (m_vColorAttachments.size() > 1)
        {
            FOX_CORE_ASSERT(m_vColorAttachments.size() <= 4, "Invalid Color attachment size");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            GLCall(glDrawBuffers(m_vColorAttachments.size(), buffers));
        }
        else if (m_vColorAttachments.empty())
        {
            // Only depth-pass
            GLCall(glDrawBuffer(GL_NONE));
        }

        FOX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}