//
// Created by samuel on 30/06/2021.
//

#include <FileSystem/FPaths.hpp>
#include "Utils/PlatformUtils.hpp"
#include "FileSystem/portable-file-dialogs.hpp"

#if _WIN32
#define DEFAULT_PATH "C:\\"
#else
#define DEFAULT_PATH "/tmp"
#endif

namespace fox
{
    std::string FileDialogs::OpenFile(const std::vector<std::string>& filter)
    {
        const std::string& path = FPaths::AssetsDir().empty() ? DEFAULT_PATH : FPaths::AssetsDir();
        // Check that a backend is available
        if (!pfd::settings::available())
            throw std::runtime_error("Portable File Dialogs are not available on this platform.");

        // File open
        auto filepath = pfd::open_file("Choose a file", path, filter);
        if (filepath.result().empty())
            return std::string();
        return filepath.result()[0];
    }

    std::string FileDialogs::SaveFile(const std::vector<std::string>& filter)
    {
        const std::string& path = FPaths::AssetsDir().empty() ? DEFAULT_PATH : FPaths::AssetsDir();

        if (!pfd::settings::available())
            throw std::runtime_error("Portable File Dialogs are not available on this platform.");

        auto filepath = pfd::save_file("Save File", path, filter);
        return filepath.result();
    }
}