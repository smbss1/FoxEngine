/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** SharedLib.hpp header
*/

#ifndef FOX_SHARED_LIB_HPP_
#define FOX_SHARED_LIB_HPP_

#include <iostream>
#include <string>
#include <stdexcept>

#ifdef __linux__
#include "dlfcn.h"
#elif __APPLE__
#include "dlfcn.h"
#elif _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#define DL_TYPE void *
#define DL_OPEN(libname) dlopen((libname), RTLD_LAZY)
#define DL_CLOSE(lib) dlclose((lib))
#define DL_SYM(lib, fn) dlsym((lib), (fn))
#define DL_EXT ".so"
#elif __APPLE__
#define DL_TYPE void *
#define DL_OPEN(libname) dlopen((libname), RTLD_LAZY)
#define DL_CLOSE(lib) dlclose((lib))
#define DL_SYM(lib, fn) dlsym((lib), (fn))
#define DL_EXT ".dylib"
#elif _WIN32
#define DL_TYPE HINSTANCE
#define DL_OPEN(libname) LoadLibraryA(libname)
#define DL_CLOSE(lib) FreeLibrary((lib))
#define DL_SYM(lib, fn) GetProcAddress((lib), (fn))
#define DL_EXT ".dll"
#endif

namespace fox
{
    class SharedLib
    {
    public:
        SharedLib();
        SharedLib(const std::string &strPath);
        ~SharedLib();

        /**
        * @brief Opens a shared library.
        *        Call `SharedLibrary::getError()` to get the error message.
        * 
        * @param path the path to the dynamic library
        * @return true on success or false and runtime_exception on error.
        */
        virtual void open(const std::string &strPath);

        /**
        * @brief Closes the shared library.
        */
        virtual void close();

        /**
        * @brief Retrieves a data pointer from a dynamic library.
        *        Call `SharedLibrary::getError()` to get the error message.
        * @return true on success or false and runtime_exception if the symbol was not found.
        */
        template <typename Signature>
        bool sym(const std::string &strName, Signature *&pSymbol)
        {
            if (!m_pHandle)
                throwError("Cannot find symbol in a null library");
            void *ptr = DL_SYM(m_pHandle, strName.c_str());
            if (!ptr)
            {
                throwError("Symbol '" + strName + "' not found.");
                return false;
            }
            pSymbol = reinterpret_cast<Signature *>(ptr);
            return true;
        }

        /**
        * @brief Get the error message
        * @return the error message
        */
        std::string getError() const;
        std::string getPath() const;
        DL_TYPE GetHandle() const;

        void reload();

    public:
    protected:
        /**
        * @brief Throw an runtime_exception
        */
        void throwError(const std::string &strMsg);

        DL_TYPE m_pHandle;
        std::string m_strError;
        std::string m_strPath;
    };
}

#endif //! FOX_SHARED_LIB_HPP_