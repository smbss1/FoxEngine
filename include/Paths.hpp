
#pragma once

#include <string>

namespace Fox
{
    namespace Path
    {
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

        inline std::string RemoveExtension(std::string& strFilename)
        {
            size_t lastindex = strFilename.find_last_of("."); 
            return strFilename.substr(0, lastindex); 
        }
    } // namespace Path
} // namespace Fox