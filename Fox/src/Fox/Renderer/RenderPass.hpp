//
// Created by samuel on 29/10/22.
//

#ifndef FOX_LEXER_RENDERPASS_HPP
#define FOX_LEXER_RENDERPASS_HPP

#include "Framebuffer.hpp"

namespace fox
{
    struct RenderPassSpecification
    {
        Ref<Framebuffer> RenderTarget;
    };

    class RenderPass : public RefCounted
    {
    public:
        RenderPass(const RenderPassSpecification& specification);
        const RenderPassSpecification& GetSpecs() const { return m_Specs; }
        RenderPassSpecification GetSpecs() { return m_Specs; }

        static Ref<RenderPass> Create(const RenderPassSpecification& specification);
    private:
        RenderPassSpecification m_Specs;
    };
}

#endif //FOX_LEXER_RENDERPASS_HPP
