//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_FRAMEBUFFER_HPP
#define FOXENGINE_FRAMEBUFFER_HPP

#include <vector>
#include "common.hpp"
#include "glm/vec4.hpp"

namespace fox
{
    enum class FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format)
                : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
                : Attachments(attachments) {}
//        FramebufferAttachmentSpecification& operator=(std::initializer_list<FramebufferTextureSpecification> attachments)
//        {
//            Attachments = attachments;
//        }

        std::vector<FramebufferTextureSpecification> Attachments{};
    };

    struct FramebufferSpecification
    {
        uint32_t Width = 0, Height = 0;
        FramebufferAttachmentSpecification Attachments;
        uint32_t Samples = 1;

        glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        float DepthClearValue = 0.0f;
        bool ClearColorOnLoad = true;
        bool ClearDepthOnLoad = true;


        bool SwapChainTarget = false;
    };

    class Framebuffer : public RefCounted
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
        virtual float ReadDepthPixel(int x, int y) = 0;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

        [[nodiscard]] virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const = 0;

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    };

}


#endif //FOXENGINE_FRAMEBUFFER_HPP
