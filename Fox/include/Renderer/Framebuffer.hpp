//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_FRAMEBUFFER_HPP
#define FOXENGINE_FRAMEBUFFER_HPP


namespace fox
{
    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
//        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

//        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
//
//        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

//        virtual const FramebufferSpecification& GetSpecification() const = 0;

//        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    };
}


#endif //FOXENGINE_FRAMEBUFFER_HPP
