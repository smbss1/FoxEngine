//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_BASE_HPP
#define FOXENGINE_BASE_HPP

#include "DebugBreak.hpp"

#ifdef FOX_DEBUG
    #define FOX_DEBUGBREAK() debug_break()
	#define FOX_ENABLE_ASSERTS
#else
    #define FOX_DEBUGBREAK()
#endif

#define FOX_EXPAND_MACRO(x) x
#define FOX_STRINGIFY_MACRO(x) #x

#endif //FOXENGINE_BASE_HPP
