//
// Created by samuel on 17/07/2021.
//

#include <rttr/registration>
#include "glm/glm.hpp"

RTTR_REGISTRATION
{
    rttr::registration::class_<glm::vec2>("vec2")
            .constructor()
            .property("x", &glm::vec2::x)
            .property("y", &glm::vec2::y);

    rttr::registration::class_<glm::vec3>("vec3")
            .constructor()
            .property("x", &glm::vec3::x)
            .property("y", &glm::vec3::y)
            .property("z", &glm::vec3::z);

    rttr::registration::class_<glm::vec4>("vec4")
            .constructor()
            .property("x", &glm::vec4::x)
            .property("y", &glm::vec4::y)
            .property("z", &glm::vec4::z)
            .property("w", &glm::vec4::w);
}