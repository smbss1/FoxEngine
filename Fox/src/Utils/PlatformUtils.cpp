//
// Created by samuel on 30/06/2021.
//

#include "Utils/PlatformUtils.hpp"
#include "portable-file-dialogs.hpp"

#if _WIN32
#define DEFAULT_PATH "C:\\"
#else
#define DEFAULT_PATH "/tmp"
#endif

namespace fox
{
    std::string FileDialogs::OpenFile(const std::vector<std::string>& filter)
    {
        // Check that a backend is available
        if (!pfd::settings::available())
            throw std::runtime_error("Portable File Dialogs are not available on this platform.");

        // File open
        auto filepath = pfd::open_file("Choose a file", DEFAULT_PATH, filter);
        if (filepath.result().empty())
            return std::string();
        return filepath.result()[0];
    }

    std::string FileDialogs::SaveFile(const std::vector<std::string>& filter)
    {
        if (!pfd::settings::available())
            throw std::runtime_error("Portable File Dialogs are not available on this platform.");

        auto filepath = pfd::save_file("Save File", DEFAULT_PATH, filter);
        return filepath.result();
    }
}