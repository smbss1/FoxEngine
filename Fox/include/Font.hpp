//
// Created by samuel on 08/07/2021.
//

#ifndef FOXENGINE_FONT_HPP
#define FOXENGINE_FONT_HPP

#include <string>

namespace fox
{
    class Font
    {
    public:
        Font(const std::string& strFilepath, int fontSize);

    private:

        std::string m_strFilepath;
        int m_iFontSize;

        void GenerateBitmap();
    };
}


#endif //FOXENGINE_FONT_HPP
