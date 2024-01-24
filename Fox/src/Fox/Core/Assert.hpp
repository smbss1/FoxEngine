//
// Created by samuel on 27/06/2021.
//

#include "Base.hpp"
#include "Logger/Logger.hpp"
#include "Debug/DebugBreak.hpp"

#ifndef FOXENGINE_ASSERT_HPP
#define FOXENGINE_ASSERT_HPP


#ifdef FOX_DEBUG
    #define FOX_DEBUGBREAK __debugbreak()
	#define FOX_ENABLE_ASSERTS
#else
    #define FOX_DEBUGBREAK
#endif

#ifdef FOX_ENABLE_ASSERTS

	#define FOX_CORE_ASSERT_MESSAGE_INTERNAL(...)  ::fox::Log::PrintAssertMessage(::fox::Log::Type::Core, "Assertion Failed", __VA_ARGS__)
	#define FOX_ASSERT_MESSAGE_INTERNAL(...)  ::fox::Log::PrintAssertMessage(::fox::Log::Type::Client, "Assertion Failed", __VA_ARGS__)

	#define FOX_CORE_ASSERT(condition, ...) { if(!(condition)) { FOX_CORE_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); FOX_DEBUGBREAK; } }
	#define FOX_ASSERT(condition, ...) { if(!(condition)) { FOX_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); FOX_DEBUGBREAK; } }

#else
    #define FOX_ASSERT(...)
    #define FOX_CORE_ASSERT(...)
#endif


#ifdef FOX_ENABLE_VERIFY
    #define FOX_CORE_VERIFY_MESSAGE_INTERNAL(...)  fox::error("Verify Failed", msg, __VA_ARGS__)

    // this the client version
	#define FOX_VERIFY_MESSAGE_INTERNAL(...)  fox::error("Verify Failed", msg, __VA_ARGS__)

	#define FOX_CORE_VERIFY(condition, ...) { if(!(condition)) { FOX_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); FOX_DEBUGBREAK; } }
	#define FOX_VERIFY(condition, ...) { if(!(condition)) { FOX_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); FOX_DEBUGBREAK; } }
#else
    #define FOX_CORE_VERIFY(condition, ...)
    #define FOX_VERIFY(condition, ...)
#endif

#endif //FOXENGINE_ASSERT_HPP
