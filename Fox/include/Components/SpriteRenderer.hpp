

#ifndef FOX_SPRITE_RENDERER_HPP_
#define FOX_SPRITE_RENDERER_HPP_

#include <string>
#include <glm/glm.hpp>
#include <Renderer/SubTexture2D.hpp>
#include <rttr/type>
#include <Reflection.hpp>
#include <Components/Component.hpp>

class SpriteRenderer : public ComponentImpl<SpriteRenderer>
{
    REFLECTABLEV(SpriteRenderer, Component)

public:
    SpriteRenderer() = default;
    explicit SpriteRenderer(const fox::ref<fox::Texture2D>& sprite, const glm::vec4& color) : Color(color), m_pSprite(sprite) {}
    explicit SpriteRenderer(const glm::vec4& color) : Color(color)
    {
    }

    std::string m_strFilepath;

    fox::ref<fox::Texture2D> m_pSprite = nullptr;
    int Depth = 0;
    glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

#endif