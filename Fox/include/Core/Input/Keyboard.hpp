//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_KEYBOARD_HPP
#define ECS_KEYBOARD_HPP

#include "KeyCodes.hpp"
#include "InputRegistry.hpp"

namespace fox
{
    struct KeyboardEvent
    {
        Key key;
        InputState state;
    };

    class Keyboard : public InputRegistry<Key>
    {
    public:
        void handle_keyboard_event(const KeyboardEvent &event);
    };
}


#endif //ECS_KEYBOARD_HPP
