

#ifndef FOX_SPRITE_RENDERER_HPP_
#define FOX_SPRITE_RENDERER_HPP_

#include <string>
#include <glm/glm.hpp>
#include <Renderer/SubTexture2D.hpp>
#include <Core/Property.hpp>
#include "Core/Logger/Logger.hpp"

namespace fox
{
    struct SpriteRenderer
    {
        SpriteRenderer() = default;
        SpriteRenderer(const SpriteRenderer&) = default;

        fox::Ref<fox::Texture2D> Sprite;
        glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
        float TilingFactor = 1.0f;
        int Depth = 0;
    };

    struct CircleRenderer
    {
        glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Thickness = 1.0f;
        float Fade = 0.005f;

        CircleRenderer() = default;
        CircleRenderer(const CircleRenderer&) = default;
    };
}

#endif
