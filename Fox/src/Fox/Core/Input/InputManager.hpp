//
//
//#ifndef FOX_INPUT_HPP_
//#define FOX_INPUT_HPP_
//
//#include <unordered_map>
//#include <Core/Input/Keyboard.hpp>
//#include <Core/Logger/Logger.hpp>
//#include "KeyCodes.hpp"
//#include "MouseCodes.hpp"
//#include "Mouse.hpp"
//
//namespace fox
//{
//    enum class InputEventType
//    {
//        KEYBOARD,
//        MOUSE,
//    };
//
//    struct InputEvent
//    {
//        InputEventType type;
//        union
//        {
//            KeyboardEvent keyboard;
//            MouseEvent mouse;
//        };
//    };
//
//    class InputManager
//    {
//	public:
//
//        bool GetKey(KeyCode key)
//        {
//            return m_oKeyboard.IsDown(key);
//        }
//
//		bool GetKeyDown(KeyCode key)
//        {
//            return m_oKeyboard.IsPressed(key);
//        }
//
//		bool GetKeyUp(KeyCode key)
//        {
//            return m_oKeyboard.IsReleased(key);
//        }
//
//        bool GetMouseButton(MouseButton key)
//        {
//            return m_oMouse.IsDown(key);
//        }
//
//        bool GetMouseButtonDown(MouseButton key)
//        {
//            return m_oMouse.IsPressed(key);
//        }
//
//        bool GetMouseButtonUp(MouseButton key)
//        {
//            return m_oMouse.IsReleased(key);
//        }
//
//        const Vec2 &GetMousePosition() const
//        {
//            return m_oMouse.GetPosition();
//        }
//
//        const Vec2 &GetMouseMovement() const
//        {
//            return m_oMouse.GetMovement();
//        }
//
//        const Vec2 &GetMouseScroll() const
//        {
//            return m_oMouse.GetScroll();
//        }
//
//        float GetMouseScrollDelta() const
//        {
//            return m_oMouse.GetScrollDelta();
//        }
//
//        void Reset()
//        {
//            m_oKeyboard.Clear();
//            m_oMouse.Clear();
//        }
//
//        void PushEvent(const InputEvent& event)
//        {
//            switch (event.type) {
//                case InputEventType::KEYBOARD:
//                    m_oKeyboard.handle_keyboard_event(event.keyboard);
//                    break;
//                case InputEventType::MOUSE:
//                    m_oMouse.HandleEvent(event.mouse);
//                    break;
//                default:
//                    break;
//            }
//        }
//
//        Keyboard m_oKeyboard;
//        Mouse m_oMouse;
//	};
//}
//
//#endif
