//
// Created by samuel on 27/06/2021.
//

#include "Base.hpp"
#include "Logger/Logger.hpp"
#include "Debug/DebugBreak.hpp"

#ifndef FOXENGINE_ASSERT_HPP
#define FOXENGINE_ASSERT_HPP


#ifdef FOX_DEBUG
    #define FOX_DEBUGBREAK debug_break()
	#define FOX_ENABLE_ASSERTS
#else
    #define FOX_DEBUGBREAK
#endif

#ifdef FOX_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define FOX_INTERNAL_ASSERT_IMPL(check, msg, ...) { if(!(check)) { FOX_CORE_ERROR(msg, __VA_ARGS__); FOX_DEBUGBREAK; } }
	#define FOX_INTERNAL_ASSERT_WITH_MSG(check, ...) FOX_INTERNAL_ASSERT_IMPL(check, "Assertion failed: %", __VA_ARGS__)
	#define FOX_INTERNAL_ASSERT_NO_MSG(check) FOX_INTERNAL_ASSERT_IMPL(check, "Assertion '%' failed at %:%", FOX_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define FOX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define FOX_INTERNAL_ASSERT_GET_MACRO(...) FOX_EXPAND_MACRO( FOX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, FOX_INTERNAL_ASSERT_WITH_MSG, FOX_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define FOX_ASSERT(...) FOX_EXPAND_MACRO( FOX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__) )
	#define FOX_CORE_ASSERT(...) FOX_EXPAND_MACRO( FOX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
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
