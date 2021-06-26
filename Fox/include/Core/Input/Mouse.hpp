//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_MOUSE_HPP
#define ECS_MOUSE_HPP

#include <Utils/Vec2.hpp>
#include "InputRegistry.hpp"
#include "MouseCodes.hpp"

namespace fox
{
    enum class MouseEventType
    {
        MOVE,
        BUTTON,
        SCROLL,
    };

    struct MouseMoveEvent
    {
        float x;
        float y;
    };

    struct MouseButtonEvent
    {
        MouseButton button;
        InputState state;
    };

    struct MouseScrollEvent
    {
        float x;
        float y;
        float delta;
    };

    struct MouseEvent
    {
        MouseEventType type;
        union
        {
            MouseMoveEvent pos;
            MouseButtonEvent button;
            MouseScrollEvent scroll;
        };
    };

    class Mouse : public InputRegistry<MouseButton>
    {
    public:
        void Clear() override;

        const Vec2 &GetPosition() const;

        const Vec2 &GetMovement() const;

        const Vec2 &GetScroll() const;
        float GetScrollDelta() const;

        void HandleEvent(const MouseEvent &event);

    private:
        void HandleEvent(const MouseButtonEvent &event);

        void HandleEvent(const MouseMoveEvent &event);

        void HandleEvent(const MouseScrollEvent &event);

        Vec2 m_oPos {0.0f, 0.0f};
        Vec2 m_oMovement {0.0f, 0.0f};
        Vec2 m_oScroll {0.0f, 0.0f};
        float m_oScrollDelta = 0.0f;
    };
}

#endif //ECS_MOUSE_HPP
