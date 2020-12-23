
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

namespace Fox
{
    namespace Path
    {
        using Paths = std::vector<std::string>;

        // "/foo/bar/baz.txt" --> "/foo/bar/"
        inline std::string GetPath( const std::string& path, char delimiter = '/' )
        {
            return path.substr( 0, path.find_last_of( delimiter ) + 1 );
        }

        // "/foo/bar/baz.txt" --> "baz.txt"
        inline std::string GetFilename(const std::string& strFilepath, std::string strDelimiter = "/")
        {
            return strFilepath.substr(strFilepath.find_last_of(strDelimiter) + 1);
        }

        inline std::string RemoveExtension(std::string& strFilename)
        {
            size_t lastindex = strFilename.find_last_of("."); 
            return strFilename.substr(0, lastindex); 
        }

        // "/foo/bar/baz.txt" --> "baz"
        inline std::string GetBaseFilename(const std::string& strFilepath, std::string strDelimiter = "/")
        {
            std::string strFilename = strFilepath.substr(strFilepath.find_last_of(strDelimiter) + 1);
            return RemoveExtension(strFilename);
        }

        // "/foo/bar/baz.txt" --> ".txt"
        // "/foo/bar/baz" --> ""
        inline std::string GetFileExtension(const std::string& strFilepath, std::string strDelimiter = "/")
        {
            std::string strFilename = GetFilename(strFilepath, strDelimiter);
            std::string::size_type n = strFilename.find_last_of('.');
            if (n != std::string::npos)
                return strFilename.substr(n);
            return std::string();
        }

        /**
        * @brief Get all files containing in a directory
        * @param strPath the path of the directory
        * @return a list of all the files paths in the specific directory
        */
        inline Paths GetAllFilesInDir(const std::string& strPath, bool bRecurse = false)
        {
            Paths vFiles;
            if (fs::exists(strPath) && fs::is_directory(strPath))
            {
                if (bRecurse)
                {
                    for (auto const & entry : fs::recursive_directory_iterator(strPath))
                        if (fs::is_regular_file(entry))
                            vFiles.emplace_back(entry.path().filename());
                }
                else
                {
                    for (const auto & entry : fs::directory_iterator(strPath))
                        if (fs::is_regular_file(entry))
                            vFiles.emplace_back(entry.path().filename());
                }
            }
            return vFiles;
        }

        /**
        * @brief Get all files containing in a directory
        * @param strPath the path of the directory
        * @return a list of all the files paths in the specific directory
        */
        inline bool PathExist(const std::string& strPath)
        {
            return fs::exists(strPath);
        }

        /**
        * @brief   Return the filenames of all files that have the specified extension
        *          in the specified directory and all subdirectories (if needed).
        * @param strPath the path of the directory
        * @param strExt the extension to be find
        * @param bRecurse if true it will get all files of the specific directory and subdirectories
        *                   otherwise it will get only all files of the specific directory.
        * @return a list of all the files paths in the specific directory and all subdirectories (if needed).
        */
        Paths GetAllFilesInDir(const std::string& strPath, const std::string& strExt, bool bRecurse = false)
        {
            Paths paths;

            if (fs::exists(strPath) && fs::is_directory(strPath))
            {
                if (bRecurse)
                {
                    for (auto const & entry : fs::recursive_directory_iterator(strPath))
                        if (fs::is_regular_file(entry) && entry.path().extension() == strExt)
                            paths.emplace_back(entry.path().filename());
                }
                else
                {
                    for (const auto & entry : fs::directory_iterator(strPath))
                        if (fs::is_regular_file(entry) && entry.path().extension() == strExt)
                            paths.emplace_back(entry.path().filename());
                }
                
            }

            return paths;
        }
    } // namespace Path
} // namespace Fox