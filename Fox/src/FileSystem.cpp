/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** FileSystem.cpp
*/

#include "Utils/FileSystem.hpp"

namespace fox
{
    namespace file
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
    } // namespace file
} // namespace fox