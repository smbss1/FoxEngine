/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.hpp header
*/

#ifndef LIBSFML_H_
#define LIBSFML_H_

#include <unordered_map>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>

#include "api.hpp"
#include "IGraphic.hpp"
#include "KeyCodes.hpp"

namespace fox
{
    class LibSFML : public IGraphic
    {
    public:
        static std::unordered_map<sf::Keyboard::Key, fox::KeyCode> SFMLKeyEventMap;

        LibSFML();
        ~LibSFML();
        // void init(IScene &oScene) override;
        // void update(IScene &oScene, float dt) override;
        // void end(IScene &oScene) override;

        // bool mousePressed(arcade::event::MouseEvent& mouse);

        void init() override;
        void shutdown() override;
        void poll_event(Input&) override;
        void draw() override;
        void set_vsync(bool value) override;
        bool is_vsync() override;
        bool quit_requested() override;

    private:
        sf::RenderWindow m_oWindow;
        sf::Event m_oEvent;

        // sf::Font m_font;

        bool m_bIsVSync;
        bool m_bMouseLeft, m_bWasMouseLeft;
        bool m_bMouseRight, m_bWasMouseRight;
        bool m_bMouseMiddle, m_bWasMouseMiddle;
        bool m_bIsQuit;
    };

    // static sf::Color getColor(arcade::Color color);

    std::unordered_map<sf::Keyboard::Key, fox::KeyCode> LibSFML::SFMLKeyEventMap = {
        {sf::Keyboard::A, fox::Key::A},
        {sf::Keyboard::B, fox::Key::B},
        {sf::Keyboard::C, fox::Key::C},
        {sf::Keyboard::D, fox::Key::D},
        {sf::Keyboard::E, fox::Key::E},
        {sf::Keyboard::F, fox::Key::F},
        {sf::Keyboard::G, fox::Key::G},
        {sf::Keyboard::H, fox::Key::H},
        {sf::Keyboard::I, fox::Key::I},
        {sf::Keyboard::J, fox::Key::J},
        {sf::Keyboard::K, fox::Key::K},
        {sf::Keyboard::L, fox::Key::L},
        {sf::Keyboard::M, fox::Key::M},
        {sf::Keyboard::N, fox::Key::N},
        {sf::Keyboard::O, fox::Key::O},
        {sf::Keyboard::P, fox::Key::P},
        {sf::Keyboard::Q, fox::Key::Q},
        {sf::Keyboard::R, fox::Key::R},
        {sf::Keyboard::S, fox::Key::S},
        {sf::Keyboard::T, fox::Key::T},
        {sf::Keyboard::U, fox::Key::U},
        {sf::Keyboard::V, fox::Key::V},
        {sf::Keyboard::W, fox::Key::W},
        {sf::Keyboard::X, fox::Key::X},
        {sf::Keyboard::Y, fox::Key::Y},
        {sf::Keyboard::Z, fox::Key::Z},
        {sf::Keyboard::Num0, fox::Key::D0},
        {sf::Keyboard::Num1, fox::Key::D1},
        {sf::Keyboard::Num2, fox::Key::D2},
        {sf::Keyboard::Num3, fox::Key::D3},
        {sf::Keyboard::Num4, fox::Key::D4},
        {sf::Keyboard::Num5, fox::Key::D5},
        {sf::Keyboard::Num6, fox::Key::D6},
        {sf::Keyboard::Num7, fox::Key::D7},
        {sf::Keyboard::Num8, fox::Key::D8},
        {sf::Keyboard::Num9, fox::Key::D9},
        {sf::Keyboard::Numpad0, fox::Key::KP0},
        {sf::Keyboard::Numpad1, fox::Key::KP1},
        {sf::Keyboard::Numpad2, fox::Key::KP2},
        {sf::Keyboard::Numpad3, fox::Key::KP3},
        {sf::Keyboard::Numpad4, fox::Key::KP4},
        {sf::Keyboard::Numpad5, fox::Key::KP5},
        {sf::Keyboard::Numpad6, fox::Key::KP6},
        {sf::Keyboard::Numpad7, fox::Key::KP7},
        {sf::Keyboard::Numpad8, fox::Key::KP8},
        {sf::Keyboard::Numpad9, fox::Key::KP9},
        {sf::Keyboard::F1, fox::Key::F1},
        {sf::Keyboard::F2, fox::Key::F2},
        {sf::Keyboard::F3, fox::Key::F3},
        {sf::Keyboard::F4, fox::Key::F4},
        {sf::Keyboard::F5, fox::Key::F5},
        {sf::Keyboard::F6, fox::Key::F6},
        {sf::Keyboard::F7, fox::Key::F7},
        {sf::Keyboard::F8, fox::Key::F8},
        {sf::Keyboard::F9, fox::Key::F9},
        {sf::Keyboard::F10, fox::Key::F10},
        {sf::Keyboard::F11, fox::Key::F11},
        {sf::Keyboard::F12, fox::Key::F12},
        {sf::Keyboard::F13, fox::Key::F13},
        {sf::Keyboard::F14, fox::Key::F14},
        {sf::Keyboard::F15, fox::Key::F15},

        {sf::Keyboard::Escape, fox::Key::Escape},
        {sf::Keyboard::Backspace, fox::Key::Backspace},
        {sf::Keyboard::Space, fox::Key::Space},
        {sf::Keyboard::Left, fox::Key::Left},
        {sf::Keyboard::Right, fox::Key::Right},
        {sf::Keyboard::Up, fox::Key::Up},
        {sf::Keyboard::Down, fox::Key::Down},
        {sf::Keyboard::LShift, fox::Key::LeftShift},
        {sf::Keyboard::RShift, fox::Key::RightShift},
        {sf::Keyboard::LControl, fox::Key::LeftControl},
        {sf::Keyboard::RControl, fox::Key::RightControl},
        {sf::Keyboard::LAlt, fox::Key::LeftAlt},
        {sf::Keyboard::RAlt, fox::Key::RightAlt},
        {sf::Keyboard::PageUp, fox::Key::PageUp},
        {sf::Keyboard::PageDown, fox::Key::PageDown},
        {sf::Keyboard::Delete, fox::Key::Delete},
        {sf::Keyboard::Insert, fox::Key::Insert},
        {sf::Keyboard::End, fox::Key::End},
        {sf::Keyboard::Semicolon, fox::Key::Semicolon},
        {sf::Keyboard::Equal, fox::Key::Equal},
        {sf::Keyboard::Comma, fox::Key::Comma},
        {sf::Keyboard::Subtract, fox::Key::Minus},
        {sf::Keyboard::Slash, fox::Key::Slash},
        {sf::Keyboard::Period, fox::Key::Period},
        {sf::Keyboard::LBracket, fox::Key::LeftBracket},
        {sf::Keyboard::RBracket, fox::Key::RightBracket},
        {sf::Keyboard::Tab, fox::Key::Tab},
        {sf::Keyboard::Home, fox::Key::Home},
        {sf::Keyboard::Multiply, fox::Key::KPMultiply},
        {sf::Keyboard::Pause, fox::Key::Pause},
        {sf::Keyboard::Add, fox::Key::KPAdd},
        {sf::Keyboard::Divide, fox::Key::KPDivide},
        {sf::Keyboard::RSystem, fox::Key::RightSuper},
        {sf::Keyboard::LSystem, fox::Key::LeftSuper},
    };
}

#endif /* !LIBSFML_H_ */