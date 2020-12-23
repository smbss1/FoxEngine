
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "Paths.hpp"

namespace Fox
{
    namespace File
    {
        /**
        * @brief Wrote file
        * @param strFile the path of the file
        * @param[out] strSave the data who will be save to the file
        * @return true if the write success, otherwise false
        */
        bool WriteFile(const std::string& strFile, const std::string& strSave)
        {
            std::ofstream oFile(strFile); //open in constructor
            if (!oFile.is_open())
                return false;

            oFile << strSave;

            return true;
        }

        /**
        * @brief Read file
        * @param strFile the path of the file
        * @param[out] strOutReadContent the content of the file will be set in this variable
        * @return true if the read success, otherwise false
        */
        bool ReadFile(const std::string& strFile, std::string& strOutReadContent)
        {
            if (!Fox::Path::PathExist(strFile))
                return false;

            std::ifstream oFile(strFile); //open the input file
            if (!oFile.is_open())
                return false;

            std::stringstream strStream;
            strStream << oFile.rdbuf(); //read the file
            strOutReadContent = strStream.str();

            return true;
        }
    } // namespace File
    
} // namespace Fox
