//
// Created by samuel on 05/10/22.
//

#include "Utils/PlatformUtils.hpp"
#include <GLFW/glfw3.h>

namespace fox
{
    float Time::GetTime()
    {
        return glfwGetTime();
    }
}
