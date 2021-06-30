//
// Created by samuel on 29/06/2021.
//

#ifndef FOXENGINE_OPENGLFRAMEBUFFER_HPP
#define FOXENGINE_OPENGLFRAMEBUFFER_HPP

#include <Renderer/Framebuffer.hpp>

namespace fox
{
    class OpenGLFrameBuffer : public Framebuffer
    {
    public:
        explicit OpenGLFrameBuffer(const FramebufferSpecification &spec);

        ~OpenGLFrameBuffer() override;

        void Invalidate();

        void Bind() override;

        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;

        int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

        void ClearAttachment(uint32_t attachmentIndex, int value) override;

        uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

        [[nodiscard]] const FramebufferSpecification& GetSpecification() const override;
    private:

        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachment = 0;
        uint32_t m_DepthAttachment = 0;
        FramebufferSpecification m_oSpecs;
    };
}

#endif //FOXENGINE_OPENGLFRAMEBUFFER_HPP
