
#include "Components/Sprite.hpp"
#include "Engine.hpp"
#include "Events.hpp"

using namespace Fox;

Sprite::Sprite() : m_oTexture(NULL), m_oTextureRect()
{
    std::shared_ptr<EventManager> pEventManager = oLocator.Resolve<EventManager>();
    if (pEventManager == nullptr)
    {
        std::cerr << "EventManager is null" << std::endl;
        return;
    }
    m_lOnSceneLoadSlot = pEventManager->AddListener(Fox::Event::Engine::Scene::OnLoadScene, Simple::slot(this, &Sprite::Init));

    m_vVertices = sf::VertexArray(sf::TriangleStrip, 4);

    // define the color of the triangle's points
    setColor(sf::Color::White);

    // define it as a rectangle, located at (0, 0) and with size 100x100
    setTextureRect(sf::IntRect(0, 0, 100, 100));

    std::shared_ptr<ResourceManager> pResourceManager = oLocator.Resolve<ResourceManager>();

    if (pResourceManager == nullptr)
    {
        std::cerr << "ResourceManager is null" << std::endl;
        return;
    }
    setTexture(*pResourceManager->Get<sf::Texture>("TextureFactory", "_fail_"), true);
}

Sprite::Sprite(const std::string& strFileTexture) : m_oTexture(NULL), m_oTextureRect()
{
    m_vVertices = sf::VertexArray(sf::TriangleStrip, 4);

    // define the color of the triangle's points
    setColor(sf::Color::White);

    m_strTextureFile = strFileTexture;

    std::shared_ptr<ResourceManager> pResourceManager = oLocator.Resolve<ResourceManager>();

    if (pResourceManager == nullptr)
    {
        std::cerr << "ResourceManager is null" << std::endl;
        return;
    }
    setTexture(*pResourceManager->Get<sf::Texture>("TextureFactory", m_strTextureFile), true);
}

Sprite::Sprite(sf::Texture& oTexture) : m_oTexture(NULL), m_oTextureRect()
{
    m_vVertices = sf::VertexArray(sf::TriangleStrip, 4);

    // define the color of the triangle's points
    setColor(sf::Color::White);

    setTexture(oTexture, true);
}

void Sprite::Init(Fox::Ecs::Event& oEvent)
{
    if (!m_strTextureFile.empty())
    {
        setTexture(*oLocator.Resolve<ResourceManager>()->Get<sf::Texture>("TextureFactory", m_strTextureFile), true);
    }
}

Sprite::~Sprite()
{
    std::shared_ptr<EventManager> pEventManager = oLocator.Resolve<EventManager>();
    if (pEventManager == nullptr)
    {
        std::cerr << "World is null" << std::endl;
        return;
    }
    pEventManager->RemoveListener(Fox::Event::Engine::Scene::OnLoadScene, m_lOnSceneLoadSlot);
}

void Sprite::setTexture(const sf::Texture& texture, bool resetRect)
{
    // Recompute the texture area if requested, or if there was no valid texture & rect before
    if (resetRect || (!m_oTexture && (m_oTextureRect == sf::IntRect())))
        setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));

    // Assign the new texture
    m_oTexture = &texture;
}

void Sprite::setTextureRect(const sf::IntRect& rectangle)
{
    if (rectangle != m_oTextureRect)
    {
        m_oTextureRect = rectangle;
        updatePositions();
        updateTexCoords();
    }
}

void Sprite::setColor(const sf::Color& color)
{
    // Update the vertices' color
    m_vVertices[0].color = color;
    m_vVertices[1].color = color;
    m_vVertices[2].color = color;
    m_vVertices[3].color = color;
}

const sf::Texture* Sprite::getTexture() const
{
    return m_oTexture;
}

const sf::IntRect& Sprite::getTextureRect() const
{
    return m_oTextureRect;
}

const sf::Color& Sprite::getColor() const
{
    return m_vVertices[0].color;
}

sf::FloatRect Sprite::getLocalBounds() const
{
    float width = static_cast<float>(std::abs(m_oTextureRect.width));
    float height = static_cast<float>(std::abs(m_oTextureRect.height));

    return sf::FloatRect(0.f, 0.f, width, height);
}

sf::FloatRect Sprite::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}