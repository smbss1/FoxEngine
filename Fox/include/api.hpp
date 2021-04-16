/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** api.h header
*/

#ifndef API_HPP_
#define API_HPP_

#ifdef __cplusplus
#define PRIVATE_EXTERN_C extern "C" {
#define PRIVATE_EXTERN_C_END }
#else
#define PRIVATE_EXTERN_C
#define PRIVATE_EXTERN_C_END
#endif

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define INIT_LIB_API(class)                                      \
    PRIVATE_EXTERN_C                                             \
    void *library_create()                                       \
    {                                                            \
        printf("[" #class "] Loading " #class " library ...\n"); \
        return new class();                                      \
    }                                                            \
    void library_delete(void *library)                           \
    {                                                            \
        printf("[" #class "] " #class " closing ...\n");         \
        delete static_cast<class *>(library);                    \
    }                                                            \
    PRIVATE_EXTERN_C_END

#endif /* !API_HPP_PRIVATE */