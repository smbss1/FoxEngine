//
// Created by samuel on 08/07/2021.
//

#include "Font.hpp"

namespace fox
{

    Font::Font(const std::string &strFilepath, int fontSize)
        : m_strFilepath(strFilepath)
        , m_iFontSize(fontSize)
    {
        GenerateBitmap();
    }

    void Font::GenerateBitmap()
    {

    }
}