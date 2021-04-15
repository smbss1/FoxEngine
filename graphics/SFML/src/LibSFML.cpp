/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include "LibSFML.hpp"
#include "Input.hpp"

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

    void LibSFML::init()
    {
        std::cout << "[LibSFML] Init!" << std::endl;

        m_oWindow.create({1280, 720}, "FoxEngine");
        m_oWindow.setKeyRepeatEnabled(false);
    }

    void LibSFML::shutdown()
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

    void LibSFML::draw()
    {
        // MouseEvent mouse;

        m_oWindow.clear();
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