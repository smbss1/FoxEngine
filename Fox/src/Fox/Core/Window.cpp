//
// Created by samuel on 27/06/2021.
//

#include <Core/Window.hpp>
#include "GlfwWindows/GlfwWindow.hpp"

namespace fox
{
    scope<Window> Window::Create(const WindowProps &props)
    {
        return fox::new_scope<GlfwWindow>(props);
    }
}