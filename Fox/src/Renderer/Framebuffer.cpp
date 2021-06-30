//
// Created by samuel on 29/06/2021.
//

#include <Renderer/Framebuffer.hpp>
#include <Renderer/RendererAPI.hpp>

namespace fox
{
    ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        return RendererAPI::GetGraphicPlugin()->CreateFramebuffer(spec);
    }
}
