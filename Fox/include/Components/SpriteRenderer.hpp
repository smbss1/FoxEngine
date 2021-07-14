

#ifndef FOX_SPRITE_RENDERER_HPP_
#define FOX_SPRITE_RENDERER_HPP_

#include <string>
#include <glm/glm.hpp>
#include <Renderer/SubTexture2D.hpp>
#include <rttr/type>
#include <Reflection.hpp>
#include <Components/Component.hpp>
#include <Core/Property.hpp>

class SpriteRenderer : public ComponentImpl<SpriteRenderer>
{
    REFLECTABLEV(SpriteRenderer, Component)

public:
    SpriteRenderer() = default;
    explicit SpriteRenderer(const fox::ref<fox::Texture2D>& sprite, const glm::vec4& color) : m_oColor(color), m_pSprite(sprite) {}
    explicit SpriteRenderer(const glm::vec4& color) : m_oColor(color)
    {
    }


public:
    fox::properties::rw_property<fox::ref<fox::Texture2D>> Sprite {
            GET { return m_pSprite; },
            SET { m_pSprite = value; }
    };

    fox::properties::rw_property<glm::vec4> Color {
            GET { return m_oColor; },
            SET { m_oColor = value; }
    };

private:

    fox::ref<fox::Texture2D> m_pSprite = nullptr;
    int Depth = 0;
    glm::vec4 m_oColor{ 1.0f, 1.0f, 1.0f, 1.0f };
};

#endif