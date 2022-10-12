//
// Created by samuel on 30/06/2021.
//

#include "Utils/PlatformUtils.hpp"
#include "Core/Application.hpp"
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
        const std::string& path = Application::Get().GetSpecs().WorkingDirectory.empty() ? DEFAULT_PATH : Application::Get().GetSpecs().WorkingDirectory;
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
        const std::string& path = Application::Get().GetSpecs().WorkingDirectory.empty() ? DEFAULT_PATH : Application::Get().GetSpecs().WorkingDirectory;

        if (!pfd::settings::available())
            throw std::runtime_error("Portable File Dialogs are not available on this platform.");

        auto filepath = pfd::save_file("Save File", path, filter);
        return filepath.result();
    }
}
