//
// Created by samuel on 02/07/2021.
//

#include "FPaths.hpp"

namespace fox
{
    std::string FPaths::m_strAssetDir;
    std::string FPaths::m_strProjectDir;

    const std::string& FPaths::AssetsDir()
    {
        return m_strAssetDir;
    }

    const std::string& FPaths::ProjectDir()
    {
        return m_strProjectDir;
    }

    void FPaths::SetProjectDir(const std::string& strPath)
    {
        m_strProjectDir = strPath;
        SetAssetsDir(strPath + "/assets");
    }

    void FPaths::SetAssetsDir(const std::string& strPath)
    {
        m_strAssetDir = strPath;
    }
}