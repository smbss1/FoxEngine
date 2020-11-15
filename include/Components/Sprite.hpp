
#pragma once

#include <SFML/Graphics.hpp>

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

    explicit Sprite() : m_texture(NULL), m_oTextureRect()
    {
        m_vertices = sf::VertexArray(sf::TriangleStrip, 4);

        // define the color of the triangle's points
        setColor(sf::Color::White);

        // define it as a rectangle, located at (0, 0) and with size 100x100
        setTextureRect(sf::IntRect(0, 0, 100, 100));
    }

    explicit Sprite(sf::Texture& oTexture) : m_texture(NULL), m_oTextureRect()
    {
        m_vertices = sf::VertexArray(sf::TriangleStrip, 4);

        // define the color of the triangle's points
        setColor(sf::Color::White);

        setTexture(oTexture, true);
    }

    void setTexture(const sf::Texture& texture, bool resetRect = false)
    {
        // Recompute the texture area if requested, or if there was no valid texture & rect before
        if (resetRect || (!m_texture && (m_oTextureRect == sf::IntRect())))
            setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));

        // Assign the new texture
        m_texture = &texture;
    }

    void setTextureRect(const sf::IntRect& rectangle)
    {
        if (rectangle != m_oTextureRect)
        {
            m_oTextureRect = rectangle;
            updatePositions();
            updateTexCoords();
        }
    }

    void setColor(const sf::Color& color)
    {
        // Update the vertices' color
        m_vertices[0].color = color;
        m_vertices[1].color = color;
        m_vertices[2].color = color;
        m_vertices[3].color = color;
    }

    const sf::Texture* getTexture() const
    {
        return m_texture;
    }

    const sf::IntRect& getTextureRect() const
    {
        return m_oTextureRect;
    }

    const sf::Color& getColor() const
    {
        return m_vertices[0].color;
    }

    sf::FloatRect getLocalBounds() const
    {
        float width = static_cast<float>(std::abs(m_oTextureRect.width));
        float height = static_cast<float>(std::abs(m_oTextureRect.height));

        return sf::FloatRect(0.f, 0.f, width, height);
    }

    sf::FloatRect getGlobalBounds() const
    {
        return getTransform().transformRect(getLocalBounds());
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the entity's transform -- combine it with the one that was passed by the caller
        states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

        // apply the texture
        if (m_texture)
            states.texture = m_texture;

        // you may also override states.shader or states.blendMode if you want

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    void updatePositions()
    {
        sf::FloatRect bounds = getLocalBounds();

        m_vertices[0].position = sf::Vector2f(0, 0);
        m_vertices[1].position = sf::Vector2f(0, bounds.height);
        m_vertices[2].position = sf::Vector2f(bounds.width, 0);
        m_vertices[3].position = sf::Vector2f(bounds.width, bounds.height);
    }

    void updateTexCoords()
    {
        float left   = static_cast<float>(m_oTextureRect.left);
        float right  = left + m_oTextureRect.width;
        float top    = static_cast<float>(m_oTextureRect.top);
        float bottom = top + m_oTextureRect.height;

        m_vertices[0].texCoords = sf::Vector2f(left, top);
        m_vertices[1].texCoords = sf::Vector2f(left, bottom);
        m_vertices[2].texCoords = sf::Vector2f(right, top);
        m_vertices[3].texCoords = sf::Vector2f(right, bottom);
    }

    sf::VertexArray m_vertices;
    const sf::Texture* m_texture;
    sf::IntRect m_oTextureRect;
};