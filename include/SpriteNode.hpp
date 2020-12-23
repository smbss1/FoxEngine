#pragma once

#include "Node.hpp"
#include "Components/Sprite.hpp"
#include "Core/Event.hpp"
#include "Events.hpp"

namespace Fox
{
    class SpriteNode : public Node, public sf::Drawable
    {
    public:
        SpriteNode(Engine* pEngine);
        SpriteNode(Engine* pEngine, sf::Texture& oTexture);
        void Draw(Event oParams);

        void SetTexture(const sf::Texture& oTexture, bool bResetRect = false);
        void SetTextureRect(const sf::IntRect& oRectangle);
        void SetColor(const sf::Color& oColor);

        const sf::Texture* GetTexture() const;

        const sf::IntRect& GetTextureRect() const;

        const sf::Color& GetColor() const;

        sf::FloatRect GetLocalBounds() const;

        sf::FloatRect GetGlobalBounds() const;

    private:

        virtual void draw(sf::RenderTarget& oTarget, sf::RenderStates oStates) const;
        void UpdatePositions();
        void UpdateTexCoords();

    public:
        sf::VertexArray m_vVertices;
        const sf::Texture* m_pTexture;
        sf::IntRect m_oTextureRect;
    };
}