//
// Created by samuel on 27/06/2021.
//

#include "Core/Input/Input.hpp"

namespace fox
{
    Window* Input::s_Window = nullptr;

    bool Input::IsKeyPressed(KeyCode key)
    {
        return s_Window->IsKeyPressed(key);
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

    void Input::SetWindow(Window *pWindow)
    {
        s_Window = pWindow;
    }
}
