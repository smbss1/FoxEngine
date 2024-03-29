//
// Created by samuel on 27/06/2021.
//

#include "Core/Input/Input.hpp"

namespace fox
{
    Window* Input::s_Window = nullptr;

    static std::unordered_map<KeyCode, bool> s_KeyStateMap(0);
	static std::unordered_map<Mouse, bool> s_MouseButtonStateMap(0);

	static std::vector<KeyCode> s_AllKeys =
	{
		Key::Space,
		Key::Apostrophe,
		Key::Comma,
		Key::Minus,
		Key::Period,
		Key::Slash,
		Key::D0,
		Key::D1,
		Key::D2,
		Key::D3,
		Key::D4,
		Key::D5,
		Key::D6,
		Key::D7,
		Key::D8,
		Key::D9,
		Key::Semicolon,
		Key::Equal,
		Key::A,
		Key::B,
		Key::C,
		Key::D,
		Key::E,
		Key::F,
		Key::G,
		Key::H,
		Key::I,
		Key::J,
		Key::K,
		Key::L,
		Key::M,
		Key::N,
		Key::O,
		Key::P,
		Key::Q,
		Key::R,
		Key::S,
		Key::T,
		Key::U,
		Key::V,
		Key::W,
		Key::X,
		Key::Y,
		Key::Z,
		Key::LeftBracket,
		Key::Backslash,
		Key::RightBracket,
		Key::GraveAccent,
		Key::World1,
		Key::World2,
		Key::Escape,
		Key::Enter,
		Key::Tab,
		Key::Backspace,
		Key::Insert,
		Key::Delete,
		Key::Right,
		Key::Left,
		Key::Down,
		Key::Up,
		Key::PageUp,
		Key::PageDown,
		Key::Home,
		Key::End,
		Key::CapsLock,
		Key::ScrollLock,
		Key::NumLock,
		Key::PrintScreen,
		Key::Pause,
		Key::F1,
		Key::F2,
		Key::F3,
		Key::F4,
		Key::F5,
		Key::F6,
		Key::F7,
		Key::F8,
		Key::F9,
		Key::F10,
		Key::F11,
		Key::F12,
		Key::F13,
		Key::F14,
		Key::F15,
		Key::F16,
		Key::F17,
		Key::F18,
		Key::F19,
		Key::F20,
		Key::F21,
		Key::F22,
		Key::F23,
		Key::F24,
		Key::F25,
		Key::KP0,
		Key::KP1,
		Key::KP2,
		Key::KP3,
		Key::KP4,
		Key::KP5,
		Key::KP6,
		Key::KP7,
		Key::KP8,
		Key::KP9,
		Key::KPDecimal,
		Key::KPDivide,
		Key::KPMultiply,
		Key::KPSubtract,
		Key::KPAdd,
		Key::KPEnter,
		Key::KPEqual,
		Key::LeftShift,
		Key::LeftControl,
		Key::LeftAlt,
		Key::LeftSuper,
		Key::RightShift,
		Key::RightControl,
		Key::RightAlt,
		Key::RightSuper,
		Key::Menu
	};

	static std::vector<Mouse> s_AllMouseButtons =
	{
		Mouse::Button0,
		Mouse::Button1,
		Mouse::Button2,
		Mouse::Button3,
		Mouse::Button4,
		Mouse::Button5,
		Mouse::Button6,
		Mouse::Button7
	};

    bool Input::IsKeyPressed(KeyCode key)
    {
        return s_Window->GetKey(key) && !s_KeyStateMap[key];
    }

    bool Input::IsKeyDown(KeyCode key)
    {
        return s_Window->IsKeyDown(key);
    }

    bool Input::IsKeyReleased(KeyCode key)
    {
        return !s_Window->GetKey(key) && s_KeyStateMap[key];
    }

    bool Input::IsMouseButtonPressed(Mouse button)
    {
        return s_Window->IsMouseButtonPressed(button);
    }

    glm::vec2 Input::GetMousePosition()
    {
        return s_Window->GetMousePosition();
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().y;
    }

    void Input::OnUpdate()
	{
		for (KeyCode key : s_AllKeys) {
			s_KeyStateMap[key] = s_Window->GetKey(key);
		}

		// for (Mouse mouseButton : s_AllMouseButtons) {
		// 	s_MouseButtonStateMap[mouseButton] = GetMouseButton(mouseButton);
		// }
	}

    void Input::SetWindow(Window *pWindow)
    {
        s_Window = pWindow;
    }
}
