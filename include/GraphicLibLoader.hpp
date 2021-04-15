/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** GraphicLibLoader.hpp header
*/

#ifndef FOX_GRAPHIC_LOADER_HPP_
#define FOX_GRAPHIC_LOADER_HPP_

#include <memory>
#include "SharedLib.hpp"

namespace fox
{
    class GraphicLibLoader : public SharedLib
    {
    public:
        using UPtr = std::unique_ptr<GraphicLibLoader>;
        using SPtr = std::shared_ptr<GraphicLibLoader>;
        GraphicLibLoader();
        GraphicLibLoader(const std::string &strPath);
        ~GraphicLibLoader();

        void load();
        void unload();

        template <typename T>
        T* get()
        {
            T* data = static_cast<T*>(m_pData);
            if (!data)
                throw std::runtime_error("Invalid Cast to " + std::string(typeid(T).name()));
            return data;
        }

    protected:

        void* m_pData;

        void *(*m_funcCreate)();
        void (*m_funcDestroy)(void *);
    };
} // namespace fox

#endif //! FOX_GRAPHIC_LOADER_HPP_