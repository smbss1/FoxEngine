//
// Created by samuel on 25/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "Renderer/Texture.hpp"
#include "OpenGL/OpenGLTexture.hpp"

namespace fox
{
    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        return new_ref<OpenGLTexture>(width, height);
    }

    Ref<Texture2D> Texture2D::Create(const std::string &path)
    {
        return new_ref<OpenGLTexture>(path);
    }
}
