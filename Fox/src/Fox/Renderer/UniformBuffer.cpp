//
// Created by samuel on 03/10/22.
//

#include "UniformBuffer.hpp"

#include "Renderer/Renderer.hpp"
#include "OpenGL/OpenGLUniformBuffer.hpp"

namespace fox
{
    ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
//        switch (Renderer::GetAPI())
//        {
//            case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
//            case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
//        }
//
//        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
//        return nullptr;

        return new_ref<OpenGLUniformBuffer>(size, binding);
    }

}
