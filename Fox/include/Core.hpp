/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Core
*/

#ifndef FOX_CORE_HPP_
#define FOX_CORE_HPP_

#ifdef __linux__
    #define FOX_PLATFORM_LINUX
#elif _WIN32
    #define FOX_PLATFORM_WINDOWS
#endif

#ifdef FOX_PLATFORM_WINDOWS
    #ifdef FOX_BUILD_LIB
        #define FOX_API __declspec(dllexport)
    #else
        #define FOX_API __declspec(dllimport)
    #endif
#endif

#ifdef FOX_PLATFORM_LINUX
    #ifdef FOX_BUILD_LIB
        #define FOX_API __attribute__((visibility("default")))
    #else
        #define FOX_API
    #endif
#endif

#endif /* !FOX_CORE_HPP_ */
