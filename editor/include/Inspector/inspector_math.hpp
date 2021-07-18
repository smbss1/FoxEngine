//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_DECLARE_INSPECTORS_HPP
#define FOXENGINE_DECLARE_INSPECTORS_HPP

#include <glm/vec3.hpp>
#include <Assets/Texture.hpp>
#include "inspector.hpp"

DECLARE_INSPECTOR(inspector_vec2, glm::vec2);
DECLARE_INSPECTOR(inspector_vec3, glm::vec3);
DECLARE_INSPECTOR(inspector_vec4, glm::vec4);

#endif //FOXENGINE_DECLARE_INSPECTORS_HPP
