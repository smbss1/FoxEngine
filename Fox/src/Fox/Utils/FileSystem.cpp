/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** FileSystem.cpp
*/

#include "Utils/FileSystem.hpp"

namespace fox
{
    namespace FileSystem
    {
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
    } // namespace file
} // namespace fox
