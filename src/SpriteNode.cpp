
#include "SpriteNode.hpp"


namespace Fox
{
    SpriteNode::SpriteNode(Engine* pEngine) : Node(pEngine), m_pTexture(NULL), m_oTextureRect()
    {
        pEngine->m_oWorld.AddEventListener(METHOD_LISTENER(FoxEvent::Engine::Window::OnRender, SpriteNode::Draw));

        m_vVertices = sf::VertexArray(sf::TriangleStrip, 4);

        // define the color of the triangle's points
        SetColor(sf::Color::White);

        // define it as a rectangle, located at (0, 0) and with size 100x100
        SetTextureRect(sf::IntRect(0, 0, 100, 100));
    }

    SpriteNode::SpriteNode(Engine* pEngine, sf::Texture& oTexture) : Node(pEngine), m_pTexture(NULL), m_oTextureRect()
    {
        pEngine->m_oWorld.AddEventListener(METHOD_LISTENER(FoxEvent::Engine::Window::OnRender, SpriteNode::Draw));

        m_vVertices = sf::VertexArray(sf::TriangleStrip, 4);

        // define the color of the triangle's points
        SetColor(sf::Color::White);

        SetTexture(oTexture, true);
    }

    void SpriteNode::Draw(Event oParams)
    {
        sf::RenderWindow* pWindow = oParams.GetParam<sf::RenderWindow*>(FoxEvent::Engine::Window::RENDER_WINDOW);

        pWindow->draw(*this);
    }

    void SpriteNode::SetTexture(const sf::Texture& oTexture, bool bResetRect)
    {
        // Recompute the texture area if requested, or if there was no valid texture & rect before
        if (bResetRect || (!m_pTexture && (m_oTextureRect == sf::IntRect())))
            SetTextureRect(sf::IntRect(0, 0, oTexture.getSize().x, oTexture.getSize().y));

        // Assign the new texture
        m_pTexture = &oTexture;
    }

    void SpriteNode::SetTextureRect(const sf::IntRect& oRectangle)
    {
        if (oRectangle != m_oTextureRect)
        {
            m_oTextureRect = oRectangle;
            UpdatePositions();
            UpdateTexCoords();
        }
    }

    void SpriteNode::SetColor(const sf::Color& oColor)
    {
        // Update the vertices' color
        m_vVertices[0].color = oColor;
        m_vVertices[1].color = oColor;
        m_vVertices[2].color = oColor;
        m_vVertices[3].color = oColor;
    }

    const sf::Texture* SpriteNode::GetTexture() const
    {
        return m_pTexture;
    }

    const sf::IntRect& SpriteNode::GetTextureRect() const
    {
        return m_oTextureRect;
    }

    const sf::Color& SpriteNode::GetColor() const
    {
        return m_vVertices[0].color;
    }

    sf::FloatRect SpriteNode::GetLocalBounds() const
    {
        float width = static_cast<float>(std::abs(m_oTextureRect.width));
        float height = static_cast<float>(std::abs(m_oTextureRect.height));

        return sf::FloatRect(0.f, 0.f, width, height);
    }

    sf::FloatRect SpriteNode::GetGlobalBounds() const
    {
        return m_oWorldTransform.getTransform().transformRect(GetLocalBounds());
    }

// Private
    void SpriteNode::draw(sf::RenderTarget& oTarget, sf::RenderStates oStates) const
    {
        // apply the entity's transform -- combine it with the one that was passed by the caller
        oStates.transform *= m_oWorldTransform.getTransform(); // getTransform() is defined by sf::Transformable

        // apply the texture
        if (m_pTexture)
            oStates.texture = m_pTexture;

        // you may also override states.shader or states.blendMode if you want

        // draw the vertex array
        oTarget.draw(m_vVertices, oStates);
    }

    void SpriteNode::UpdatePositions()
    {
        sf::FloatRect bounds = GetLocalBounds();

        m_vVertices[0].position = sf::Vector2f(0, 0);
        m_vVertices[1].position = sf::Vector2f(0, bounds.height);
        m_vVertices[2].position = sf::Vector2f(bounds.width, 0);
        m_vVertices[3].position = sf::Vector2f(bounds.width, bounds.height);
    }

    void SpriteNode::UpdateTexCoords()
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
} // namespace Fox
