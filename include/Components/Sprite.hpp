
#pragma once

#include <SFML/Graphics.hpp>
#include "json_struct.h"

class Event;

struct Color
{
    int r;
    int g;
    int b;
    int a;
};

struct Texture
{
public:
    explicit Texture(std::string path)
    {
        m_oTexture.loadFromFile(path);
    }
    sf::Texture m_oTexture;
};

class Sprite : public sf::Drawable, public sf::Transformable
{
public:

    Sprite();
    Sprite(const std::string& strFileTexture);
    Sprite(sf::Texture& oTexture);
    ~Sprite();
    
    void Init();
    void setTexture(const sf::Texture& texture, bool resetRect = false);
    void setTextureRect(const sf::IntRect& rectangle);
    void setColor(const sf::Color& color);
    const sf::Texture* getTexture() const;
    const sf::IntRect& getTextureRect() const;
    const sf::Color& getColor() const;
    sf::FloatRect getLocalBounds() const;
    sf::FloatRect getGlobalBounds() const;

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the entity's transform -- combine it with the one that was passed by the caller
        states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

        // apply the texture
        if (m_oTexture)
            states.texture = m_oTexture;

        // you may also override states.shader or states.blendMode if you want

        // draw the vertex array
        target.draw(m_vVertices, states);
    }

    void updatePositions()
    {
        sf::FloatRect bounds = getLocalBounds();

        m_vVertices[0].position = sf::Vector2f(0, 0);
        m_vVertices[1].position = sf::Vector2f(0, bounds.height);
        m_vVertices[2].position = sf::Vector2f(bounds.width, 0);
        m_vVertices[3].position = sf::Vector2f(bounds.width, bounds.height);
    }

    void updateTexCoords()
    {
        float left   = static_cast<float>(m_oTextureRect.left);
        float right  = left + m_oTextureRect.width;
        float top    = static_cast<float>(m_oTextureRect.top);
        float bottom = top + m_oTextureRect.height;

        m_vVertices[0].texCoords = sf::Vector2f(left, top);
        m_vVertices[1].texCoords = sf::Vector2f(left, bottom);
        m_vVertices[2].texCoords = sf::Vector2f(right, top);
        m_vVertices[3].texCoords = sf::Vector2f(right, bottom);
    }

    size_t m_lOnSceneLoadSlot;

public:
    sf::VertexArray m_vVertices;
    const sf::Texture* m_oTexture;
    sf::IntRect m_oTextureRect;
    std::string m_strTextureFile;

    JS_OBJECT(JS_MEMBER(m_strTextureFile));
};