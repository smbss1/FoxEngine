/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** GraphicLibLoader.cpp
*/

#include "GraphicLibLoader.hpp"

namespace fox
{
    GraphicLibLoader::GraphicLibLoader() : SharedLib()
    {
    }

    GraphicLibLoader::GraphicLibLoader(const std::string &strPath) : SharedLib(strPath)
    {
        m_funcCreate = nullptr;
        m_funcDestroy = nullptr;
        m_pData = nullptr;
        load();
    }

    GraphicLibLoader::~GraphicLibLoader()
    {
        if (m_pData)
        {
            (*m_funcDestroy)(m_pData);
            m_pData = nullptr;
            m_funcDestroy = nullptr;
            m_funcCreate = nullptr;
        }
    }

    /* PUBLIC */
    void GraphicLibLoader::load()
    {
        sym("library_create", m_funcCreate);
        sym("library_delete", m_funcDestroy);
        m_pData = (*m_funcCreate)();
    }

    void GraphicLibLoader::unload()
    {
        if (m_pData)
        {
            (*m_funcDestroy)(m_pData);
            m_pData = nullptr;
            m_funcDestroy = nullptr;
            m_funcCreate = nullptr;
        }
        close();
    }
}