//
// Created by samuel on 02/07/2021.
//

#ifndef FOXENGINE_FPATHS_HPP
#define FOXENGINE_FPATHS_HPP

#include <string>

namespace fox
{
    class FPaths
    {
    public:
        static const std::string& AssetsDir();
        static const std::string &ProjectDir();
        static void SetProjectDir(const std::string & strPath);
        static void SetAssetsDir(const std::string & strPath);

    private:
        static std::string m_strAssetDir;
        static std::string m_strProjectDir;
    };
}

#endif //FOXENGINE_FPATHS_HPP
