//
// Created by samuel on 25/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "Renderer/Texture.hpp"

namespace fox
{
    ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        return RendererAPI::GetGraphicPlugin()->create_texture(width, height);
    }

    ref<Texture2D> Texture2D::Create(const std::string &path)
    {
        return RendererAPI::GetGraphicPlugin()->create_texture(path);
    }
}
