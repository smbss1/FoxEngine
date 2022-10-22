//
// Created by samuel on 29/06/2021.
//

#include <Renderer/Framebuffer.hpp>
#include <Renderer/RendererAPI.hpp>
#include "OpenGL/OpenGLFrameBuffer.hpp"

namespace fox
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        return new_ref<OpenGLFrameBuffer>(spec);
    }
}
