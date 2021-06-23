//
// Created by samuel on 22/06/2021.
//

#include "Core/Input/Mouse.hpp"

namespace fox
{
    void Mouse::Clear()
    {
        InputRegistry::Clear();
        m_oScroll = {0.0f, 0.0f};
        m_oMovement = {0.0f, 0.0f};
    }

    const Vec2 &Mouse::GetPosition() const
    {
        return m_oPos;
    }

    const Vec2 &Mouse::GetMovement() const
    {
        return m_oMovement;
    }

    const Vec2 &Mouse::GetScroll() const
    {
        return m_oScroll;
    }

    void Mouse::HandleEvent(const MouseEvent &event)
    {
        switch (event.type) {
            case MouseEventType::MOVE:
                return HandleEvent(event.pos);
            case MouseEventType::BUTTON:
                return HandleEvent(event.button);
            case MouseEventType::SCROLL:
                return HandleEvent(event.scroll);
            default:
                break;
        }
    }

    void Mouse::HandleEvent(const MouseButtonEvent &event)
    {
        SetState(event.button, event.state);
    }

    void Mouse::HandleEvent(const MouseMoveEvent &event)
    {
        Vec2 new_pos {event.x, event.y};

        m_oMovement = new_pos - m_oPos;
        m_oPos = new_pos;
    }

    void Mouse::HandleEvent(const MouseScrollEvent &event)
    {
        m_oScroll = {event.x, event.y};
    }
}