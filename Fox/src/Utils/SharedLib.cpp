/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** SharedLib.cpp
*/

#include "SharedLib.hpp"

namespace fox
{
    SharedLib::SharedLib() : m_pHandle(nullptr)
    {
    }

    SharedLib::SharedLib(const std::string &strPath) : SharedLib()
    {
        open(strPath);
    }

    SharedLib::~SharedLib()
    {
        if (m_pHandle)
        {
            DL_CLOSE(m_pHandle);
            m_pHandle = nullptr;
        }
    }

    /* PUBLIC */
    void SharedLib::open(const std::string &strPath)
    {
#ifndef _WIN32
        dlerror();
#endif
        m_strError.clear();
        m_strPath = strPath;
        m_pHandle = DL_OPEN(strPath.c_str());
        if (!m_pHandle)
            throwError("Cannot load library");
    }

    void SharedLib::close()
    {
        m_strError.clear();
        if (!m_pHandle)
            throwError("Cannot close an null library");
        /* Ignore errors. No good way to signal them without leaking memory. */
        DL_CLOSE(m_pHandle);
        m_pHandle = nullptr;
    }

    std::string SharedLib::getPath() const
    {
        return m_strPath;
    }

    std::string SharedLib::getError() const
    {
        return m_strError;
    }

    /* PRIVATE */

    void SharedLib::throwError(const std::string &strMsg)
    {
// m_strError = m_strPath + ": " + strMsg;
#ifdef _WIN32
        m_strError = m_strPath + ": " + strMsg + ", return code " + std::to_string(GetLastError());
#else
        char *str = dlerror();
        if (str)
            m_strError = str;
#endif
        throw std::runtime_error(m_strError);
    }
}