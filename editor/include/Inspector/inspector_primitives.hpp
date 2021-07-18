//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_DECLARE_INSPECTORS_HPP
#define FOXENGINE_DECLARE_INSPECTORS_HPP

#include <Assets/Texture.hpp>
#include "inspector.hpp"

DECLARE_INSPECTOR(inspector_int, int);
DECLARE_INSPECTOR(inspector_float, float);
DECLARE_INSPECTOR(inspector_bool, bool);
DECLARE_INSPECTOR(inspector_stdstring, std::string);
DECLARE_INSPECTOR(inspector_string, char*);

#endif //FOXENGINE_DECLARE_INSPECTORS_HPP
