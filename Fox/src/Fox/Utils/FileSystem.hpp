/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** FileSystem.hpp
*/

#ifndef ECS_FILESYSTEM_HPP
#define ECS_FILESYSTEM_HPP


#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

namespace fox
{
    namespace file
    {
        /**
        * @brief Wrote file
        * @param strFile the path of the file
        * @param[out] strSave the data who will be save to the file
        * @return true if the write success, otherwise false
        */
        bool WriteFile(const std::string& strFile, const std::string& strSave);

        /**
        * @brief Read file
        * @param strFile the path of the file
        * @param[out] strOutReadContent the content of the file will be set in this variable
        * @return true if the read success, otherwise false
        */
        bool ReadFile(const std::string& strFile, std::string& strOutReadContent);
    } // namespace file

} // namespace fox

#endif //ECS_FILESYSTEM_HPP
