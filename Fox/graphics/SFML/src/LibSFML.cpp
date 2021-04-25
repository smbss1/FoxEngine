/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include "LibSFML.hpp"
#include "Input.hpp"
#include "TextureManager.hpp"
#include "SpriteManager.hpp"
#include "Components.hpp"

INIT_LIB_API(fox::LibSFML)

namespace fox
{
    LibSFML::LibSFML()
    {
        m_bMouseLeft = false;
        m_bWasMouseLeft = false;
        m_bMouseRight = false;
        m_bWasMouseRight = false;
        m_bMouseMiddle = false;
        m_bWasMouseMiddle = false;
        m_bIsQuit = false;
    }

    LibSFML::~LibSFML()
    {
    }

    void LibSFML::init(Application& app)
    {
        std::cout << "[LibSFML] Init!" << std::endl;

        m_oWindow.create({1280, 720}, "FoxEngine");
        m_oWindow.setKeyRepeatEnabled(false);
        app.add_manager<sf::Texture>(new TextureManager(app.get_resource_manager()));
        app.add_manager<sf::Sprite>(new SpriteManager(app.get_resource_manager()));

        app.get_active()->get_world().system<SpriteRenderer>("InitSpriteRenderer")
            .kind(ecs::OnAdd).each([&](Entity e, SpriteRenderer& renderer)
        {
            app.add_asset<sf::Texture>(renderer.m_strId, renderer.m_strPath);
            app.add_asset<sf::Sprite>(renderer.m_strId, renderer.m_strPath);
        });

        app.get_active()->get_world().system<Transform, SpriteRenderer>("DrawSpriteRenderer")
            .kind(ecs::OnStore).each([&](Entity e, Transform& transform, SpriteRenderer& renderer)
        {
            sf::Sprite* sprite = app.get_asset<sf::Sprite>(renderer.m_strId);
            sprite->setPosition(transform.position.x, transform.position.y);
            m_oWindow.draw(*sprite);
        });
    }

    void LibSFML::shutdown(Application& app)
    {
        m_oWindow.close();
    }

    void LibSFML::poll_event(Input& input)
    {
        while (m_oWindow.pollEvent(m_oEvent))
        {
            switch (m_oEvent.type)
            {
            case sf::Event::Closed:
                m_bIsQuit = true;
                break;

        //     case sf::Event::Resized:
        //     {
        //         sf::FloatRect visibleArea(0, 0, m_oEvent.size.width, m_oEvent.size.height);
        //         m_oWindow.setView(sf::View(visibleArea));
        //         break;
        //     }

        //     case sf::Event::MouseButtonPressed:
        //     {
        //         mouse.x = m_oEvent.mouseButton.x;
        //         mouse.y = m_oEvent.mouseButton.y;
        //         mouse.action = MouseEvent::DOWN;

        //         if (m_oEvent.mouseButton.button == sf::Mouse::Button::Left)
        //         {
        //             mouse.button = MouseEvent::MOUSE_PRIMARY;
        //             oScene.pushEvent(mouse);
        //         }
        //         else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Right)
        //         {
        //             mouse.button = MouseEvent::MOUSE_SECONDARY;
        //             oScene.pushEvent(mouse);
        //         }
        //         else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Middle)
        //         {
        //             mouse.button = MouseEvent::MOUSE_AUXILIARY;
        //             oScene.pushEvent(mouse);
        //         }
        //         break;
        //     }

        //     case sf::Event::MouseButtonReleased:
        //     {
        //         // mouse.x = m_oEvent.mouseButton.x;
        //         // mouse.y = m_oEvent.mouseButton.y;
        //         // mouse.action = MouseEvent::RELEASED;

        //         // if (m_oEvent.mouseButton.button == sf::Mouse::Button::Left)
        //         // {
        //         //     mouse.button = MouseEvent::MOUSE_PRIMARY;
        //         //     oScene.pushEvent(mouse);
        //         // }
        //         // else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Right)
        //         // {
        //         //     mouse.button = MouseEvent::MOUSE_SECONDARY;
        //         //     oScene.pushEvent(mouse);
        //         // }
        //         // else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Middle)
        //         // {
        //         //     mouse.button = MouseEvent::MOUSE_AUXILIARY;
        //         //     oScene.pushEvent(mouse);
        //         // }
        //         break;
        //     }

        //     // case sf::Event::MouseMoved:
        //     // {
        //     //     break;
        //     // }

            case sf::Event::KeyReleased:
            {
                input.SetKeyUp(SFMLKeyEventMap[m_oEvent.key.code]);
                break;
            }

            case sf::Event::KeyPressed:
            {
                input.SetKeyDown(SFMLKeyEventMap[m_oEvent.key.code]);
                break;
            }

            default:
                break;
            }
        }

        // if (mousePressed(mouse))
        //     oScene.pushEvent(mouse);
    }

    void LibSFML::draw(Application& app)
    {
        // MouseEvent mouse;

        m_oWindow.clear();
        app.get_active()->get_world().run_phase(ecs::OnStore);
        m_oWindow.display();
    }

    void LibSFML::set_vsync(bool value)
    {
        m_bIsVSync = value;
    }

    bool LibSFML::is_vsync()
    {
        return m_bIsVSync;
    }

    bool LibSFML::quit_requested()
    {
        return m_bIsQuit;
    }


    // bool LibSFML::mousePressed(arcade::event::MouseEvent &mouse)
    // {
    //     bool ret = false;
    //     // if (!m_bWasMouseLeft && m_bMouseLeft)
    //     // {
    //     //     mouse.action = MouseEvent::PRESSED;
    //     //     mouse.button = MouseEvent::MOUSE_PRIMARY;
    //     //     ret = true;
    //     // }
    //     // if (!m_bWasMouseRight && m_bMouseRight)
    //     // {
    //     //     mouse.action = MouseEvent::PRESSED;
    //     //     mouse.button = MouseEvent::MOUSE_SECONDARY;
    //     //     ret = true;
    //     // }
    //     // if (!m_bWasMouseMiddle && m_bMouseMiddle)
    //     // {
    //     //     mouse.action = MouseEvent::PRESSED;
    //     //     mouse.button = MouseEvent::MOUSE_AUXILIARY;
    //     //     ret = true;
    //     // }
    //     // m_bWasMouseLeft = m_bMouseLeft;
    //     // m_bWasMouseRight = m_bMouseRight;
    //     // m_bWasMouseMiddle = m_bMouseMiddle;
    //     return ret;
    // }

    // sf::Color getColor(arcade::Color color)
    // {
    //     return sf::Color(color.r,
    //                         color.g,
    //                         color.b,
    //                         color.a);
    // }
}