

#ifndef FOX_SPRITE_RENDERER_HPP_
#define FOX_SPRITE_RENDERER_HPP_

#include <string>
#include <glm/glm.hpp>
#include <Renderer/SubTexture2D.hpp>
// #include <rttr/type>
#include <Reflection.hpp>
#include <Components/Component.hpp>
#include <Core/Property.hpp>

class SpriteRenderer //: public ComponentImpl<SpriteRenderer>
{
    // REFLECTABLEV(SpriteRenderer, Component)

public:
    SpriteRenderer() = default;
    explicit SpriteRenderer(const fox::ref<fox::Texture2D>& sprite, const glm::vec4& color) : Color(color), Sprite(sprite) {}
    explicit SpriteRenderer(const glm::vec4& color) : Color(color)
    {
    }


public:
    fox::ref<fox::Texture2D> Sprite;
    glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
    float TilingFactor = 1.0f;

private:
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

#endif
