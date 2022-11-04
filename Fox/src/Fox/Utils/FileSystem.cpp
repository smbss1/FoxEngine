/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** FileSystem.cpp
*/

#include "Utils/FileSystem.hpp"
#include "Core/Core.hpp"
#include "Core/Logger/Logger.hpp"

namespace fox::FileSystem
{
    bool Exists(const std::filesystem::path& filepath)
    {
        return std::filesystem::exists(filepath);
    }

    bool Exists(const std::string& filepath)
    {
        return std::filesystem::exists(std::filesystem::path(filepath));
    }

    bool WriteFile(const std::string& strFile, const std::string& strSave)
    {
        std::ofstream oFile(strFile); //open in constructor
        if (!oFile.is_open())
            return false;

        oFile << strSave;

        return true;
    }

    bool ReadFile(const std::string& strFile, std::string& strOutReadContent)
    {
        if (!Exists(strFile))
            return false;

        std::ifstream oFile(strFile); //open the input file
        if (!oFile.is_open())
            return false;

        std::stringstream strStream;
        strStream << oFile.rdbuf(); //read the file
        strOutReadContent = strStream.str();
        return true;
    }

    char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
    {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

        if (!stream)
        {
            // Failed to open the file
            return nullptr;
        }

        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        uint64_t size = end - stream.tellg();

        if (size == 0)
        {
            // File is empty
            return nullptr;
        }

        char* buffer = new char[size];
        stream.read((char*)buffer, size);
        stream.close();

        *outSize = (uint32_t)size;
        return buffer;
    }

    bool HasEnvironmentVariable(const std::string& key)
    {
#ifdef FOX_PLATFORM_WINDOWS

        HKEY hKey;
        LPCSTR keyPath = "Environment";
        LSTATUS lOpenStatus = RegOpenKeyExA(HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &hKey);

        if (lOpenStatus == ERROR_SUCCESS)
        {
            lOpenStatus = RegQueryValueExA(hKey, key.c_str(), 0, NULL, NULL, NULL);
            RegCloseKey(hKey);
        }

        return lOpenStatus == ERROR_SUCCESS;
#elifdef FOX_PLATFORM_LINUX
        return !GetEnvironmentVariable(key).empty();
#endif
    }

    bool SetEnvironmentVariable(const std::string& key, const std::string& value)
    {
#ifdef FOX_PLATFORM_WINDOWS
        HKEY hKey;
        LPCSTR keyPath = "Environment";
        DWORD createdNewKey;
        LSTATUS lOpenStatus = RegCreateKeyExA(HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &createdNewKey);
        if (lOpenStatus == ERROR_SUCCESS)
        {
            LSTATUS lSetStatus = RegSetValueExA(hKey, key.c_str(), 0, REG_SZ, (LPBYTE)value.c_str(), (DWORD)(value.length() + 1));
            RegCloseKey(hKey);

            if (lSetStatus == ERROR_SUCCESS)
            {
                SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_BLOCK, 100, NULL);
                return true;
            }
        }
        return false;
#elifdef FOX_PLATFORM_LINUX
        return setenv(key.c_str(), value.c_str(), 1) == 0;
#endif
    }

    std::string GetEnvironmentVariable(const std::string& key)
    {
#ifdef FOX_PLATFORM_WINDOWS
        HKEY hKey;
        LPCSTR keyPath = "Environment";
        DWORD createdNewKey;
        LSTATUS lOpenStatus = RegCreateKeyExA(HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &createdNewKey);
        if (lOpenStatus == ERROR_SUCCESS)
        {
            DWORD valueType;
            char* data = hnew char[512];
            DWORD dataSize = 512;
            LSTATUS status = RegGetValueA(hKey, NULL, key.c_str(), RRF_RT_ANY, &valueType, (PVOID)data, &dataSize);

            RegCloseKey(hKey);

            if (status == ERROR_SUCCESS)
            {
                std::string result(data);
                delete[] data;
                return result;
            }
        }
#endif
#ifdef FOX_PLATFORM_LINUX
        const char* val = std::getenv(key.c_str());
         if (val != nullptr) { // invalid to assign nullptr to std::string
             return val;
         }
#endif
        return std::string{};
    }
} // namespace fox
