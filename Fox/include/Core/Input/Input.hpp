//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_INPUT_HPP
#define FOXENGINE_INPUT_HPP

#include <glm/vec2.hpp>
#include <Core/Window.hpp>

namespace fox
{
    class Input
    {
        static Window* s_Window;
    public:
        static bool IsKeyPressed(Key key);

        static bool IsMouseButtonPressed(Mouse button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

        static void SetWindow(Window* pWindow);
    };
}

#endif //FOXENGINE_INPUT_HPP
