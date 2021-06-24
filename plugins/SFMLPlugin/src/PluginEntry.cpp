/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include <Core/Managers/PluginManager.hpp>
#include <Components/EntityName.hpp>
#include <ImGui/imgui-SFML.h>
#include <Time.hpp>
#include <ImGui/imgui.h>
#include "Core/Input/InputManager.hpp"
#include "TextureManager.hpp"
#include "SpriteManager.hpp"
#include "Components.hpp"
#include "PluginEntry.hpp"

namespace fox
{
    SFMLPlugin::~SFMLPlugin()
    {
        m_oWindow.close();
    }

    void SFMLPlugin::plug(Application &app)
    {
        m_oApp = &app;
        std::cout << "[SFMLPlugin] Init!" << std::endl;

        m_oWindow.create({1280, 720}, "FoxEngine");
        m_oWindow.setKeyRepeatEnabled(false);

        ImGui::SFML::Init(m_oWindow);

        ResourceManager& resource_manager = app.get<ResourceManager>().value();
        resource_manager.AddManager<sf::Texture>(new TextureManager(resource_manager));
        resource_manager.AddManager<sf::Sprite>(new SpriteManager(resource_manager));

        app.get_world().system<SpriteRenderer>("InitSpriteRenderer")
            .kind(ecs::OnAdd).each([&](Entity& e, SpriteRenderer& renderer)
               {
                   resource_manager.AddAsset<sf::Texture>(renderer.m_strId, renderer.m_strPath);
                   resource_manager.AddAsset<sf::Sprite>(renderer.m_strId, renderer.m_strPath);
               });

        app.get_world().system<Transform, SpriteRenderer>("DrawSpriteRenderer")
            .order_by([](Transform& t1, SpriteRenderer& r1, Transform& t2, SpriteRenderer& r2)
              {
                  return r1.depth < r2.depth;
              })
            .kind(ecs::OnStore).each([&](Entity& e, Transform& transform, SpriteRenderer& renderer)
                 {
                     auto* sprite = resource_manager.GetAsset<sf::Sprite>(renderer.m_strId);
                     sprite->setPosition(transform.position.x, transform.position.y);
                     m_oWindow.draw(*sprite);
                 });
    }

    void SFMLPlugin::unplug(Application &app)
    {
        ImGui::SFML::Shutdown();
    }

    void SFMLPlugin::poll_event()
    {
        InputManager& oInputManager = m_oApp->get<InputManager>().value();
        oInputManager.Reset();
        while (m_oWindow.pollEvent(m_oEvent))
        {
            ImGui::SFML::ProcessEvent(m_oEvent);
            switch (m_oEvent.type)
            {
            case sf::Event::Closed:
                m_oApp->quit();
                break;

        //     case sf::Event::Resized:
        //     {
        //         sf::FloatRect visibleArea(0, 0, m_oEvent.size.width, m_oEvent.size.height);
        //         m_oWindow.setView(sf::View(visibleArea));
        //         break;
        //     }

             case sf::Event::MouseButtonPressed:
             {
                 InputEvent event{};
                 MouseEvent oMouseEvent {};
                 MouseButtonEvent oMouseBtnEvent{};

                 // Set the type of the event is Mouse
                 event.type = InputEventType::MOUSE;

                 // Set that it's a button event
                 oMouseEvent.type = MouseEventType::BUTTON;
                 // Set that it's a click
                 oMouseBtnEvent.state = InputState::PRESSED;

                 // Is Left Click ?
                 if (m_oEvent.mouseButton.button == sf::Mouse::Button::Left)
                     oMouseBtnEvent.button = MouseButton::ButtonLeft;
                 // Is Right Click ?
                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Right)
                     oMouseBtnEvent.button = MouseButton::ButtonRight;
                 // Is Wheel Click ?
                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Middle)
                     oMouseBtnEvent.button = MouseButton::ButtonMiddle;
                 oMouseEvent.button = oMouseBtnEvent;
                 event.mouse = oMouseEvent;
                 oInputManager.PushEvent(event);
                 break;
             }

             case sf::Event::MouseButtonReleased:
             {
                 InputEvent event{};
                 MouseEvent oMouseEvent {};
                 MouseButtonEvent oMouseBtnEvent{};

                 // Set the type of the event is Mouse
                 event.type = InputEventType::MOUSE;

                 // Set that it's a button event
                 oMouseEvent.type = MouseEventType::BUTTON;
                 // Set that it's a click
                 oMouseBtnEvent.state = InputState::RELEASED;

                 // Is Left Click ?
                 if (m_oEvent.mouseButton.button == sf::Mouse::Button::Left)
                     oMouseBtnEvent.button = MouseButton::ButtonLeft;
                     // Is Right Click ?
                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Right)
                     oMouseBtnEvent.button = MouseButton::ButtonRight;
                     // Is Wheel Click ?
                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Middle)
                     oMouseBtnEvent.button = MouseButton::ButtonMiddle;
                 oMouseEvent.button = oMouseBtnEvent;
                 event.mouse = oMouseEvent;
                 oInputManager.PushEvent(event);
                 break;
             }

            case sf::Event::MouseMoved:
            {
                InputEvent event{};
                MouseEvent oMouseEvent {};
                MouseMoveEvent oMouseMoveEvent{};

                // Set the type of the event is Mouse
                event.type = InputEventType::MOUSE;

                // Set that it's a move event
                oMouseEvent.type = MouseEventType::MOVE;
                // Set the position of the mouse
                oMouseMoveEvent.x = m_oEvent.mouseMove.x;
                oMouseMoveEvent.y = m_oEvent.mouseMove.y;

                oMouseEvent.pos = oMouseMoveEvent;
                event.mouse = oMouseEvent;
                oInputManager.PushEvent(event);
                break;
            }

            case sf::Event::MouseWheelScrolled:
            {
                InputEvent event{};
                MouseEvent oMouseEvent {};
                MouseScrollEvent oMouseScrollEvent{};

                // Set the type of the event is Mouse
                event.type = InputEventType::MOUSE;

                // Set that it's a scroll event
                oMouseEvent.type = MouseEventType::SCROLL;
                // Set the position of the mouse wheel scroll
                oMouseScrollEvent.x = m_oEvent.mouseWheelScroll.x;
                oMouseScrollEvent.y = m_oEvent.mouseWheelScroll.y;

                oMouseEvent.scroll = oMouseScrollEvent;
                event.mouse = oMouseEvent;
                oInputManager.PushEvent(event);
                break;
            }

            case sf::Event::KeyReleased:
            {
                InputEvent event{};
                event.type = InputEventType::KEYBOARD;
                event.keyboard = {SFMLKeyEventMap[m_oEvent.key.code], InputState::RELEASED};
                oInputManager.PushEvent(event);
                break;
            }

            case sf::Event::KeyPressed:
            {
                InputEvent event{};
                event.type = InputEventType::KEYBOARD;
                event.keyboard = {SFMLKeyEventMap[m_oEvent.key.code], InputState::PRESSED};
                oInputManager.PushEvent(event);
                break;
            }

            default:
                break;
            }
        }
    }

    void SFMLPlugin::draw()
    {
        ImGui::SFML::Update(m_oWindow, deltaClock.restart());

        ImGui::ShowDemoWindow();

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        m_oWindow.clear();
        m_oApp->get_world().run_phase(ecs::OnStore);
        ImGui::SFML::Render(m_oWindow);
        m_oWindow.display();
    }

    void SFMLPlugin::set_vsync(bool value)
    {
        m_bIsVSync = value;
    }

    bool SFMLPlugin::is_vsync()
    {
        return m_bIsVSync;
    }

    const std::string &SFMLPlugin::get_name() const
    {
        return "SFML";
    }

    int SFMLPlugin::get_version() const
    {
        return 1;
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

extern "C" void RegisterPlugin(fox::PluginManager &pm)
{
    fox::ref<fox::SFMLPlugin> plugin = fox::new_ref<fox::SFMLPlugin>();
    pm.AddPlugin(plugin);
    pm.GetGraphics().AddGraphicsPlugin(std::move(plugin));
}