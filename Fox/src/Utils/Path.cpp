/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** Path.cpp
*/

#include "FileSystem/Path.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include <algorithm>
#include <codecvt>
#include <cstring>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>

#if defined(__linux__) || defined(__APPLE__)
    #include <dirent.h>
    #include <errno.h>
    #include <limits.h>
    #include <unistd.h>
#elif defined(_WIN32)
#include <stdexcept>
    // Fix concat error for wstring and wchar_t[]
    #ifndef UNICODE
    #define UNICODE
    #define UNICODE_WAS_UNDEFINED
    #endif
    #include <Windows.h>
    #ifdef UNICODE_WAS_UNDEFINED
    #undef UNICODE
    #endif
#endif

#if defined(_WIN32)
static const char separator = '/';
    static const wchar_t wide_separator = L'/';
    static const char colon = ':';
    static const char win_separator = '\\';
#elif defined(_POSIX_VERSION)
constexpr char separator = '/';
constexpr char dot = '.';
constexpr char colon = ':';
#endif

static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> s_wcharConverter;

std::string generic_to_native(const std::string& s)
{
#if defined(_WIN32)
    std::string strResult;

    std::replace_copy(
        s.begin(),
        s.end(),
        std::back_inserter<std::string>(strResult),
        separator,
        win_separator
    );

    return strResult;
#elif defined(_POSIX_VERSION)
    return s;
#endif
}

std::string native_to_generic(const std::string& s)
{
#if defined(_WIN32)
    std::string strResult;

    std::replace_copy(
        s.begin(),
        s.end(),
        std::back_inserter<std::string>(strResult),
        win_separator,
        separator
    );
    return strResult;
#elif defined(_POSIX_VERSION)
    return s;
#endif
}

Path::Path(const std::string& s) : m_strPath(native_to_generic(s)) { }

Path::Path(const char* s) : m_strPath(native_to_generic(s)) { }

Path::Path(const std::wstring& s) : m_strPath(native_to_generic(s_wcharConverter.to_bytes(s))) { }

Path::Path(const wchar_t* s) : m_strPath(native_to_generic(s_wcharConverter.to_bytes(std::wstring(s)))) { }

std::string Path::string() const
{
    return generic_to_native(m_strPath);
}

std::wstring Path::wstring() const
{
    return s_wcharConverter.from_bytes(generic_to_native(m_strPath));
}

// "/foo/bar/baz.txt" --> "baz.txt"
std::string Path::filename() const
{
    return m_strPath.substr(m_strPath.find_last_of(separator) + 1);
}

std::string RemoveExtension(const std::string& strFilename)
{
    size_t lastindex = strFilename.find_last_of(".");
    return strFilename.substr(0, strFilename.find_last_of("."));
}

// "/foo/bar/baz.txt" --> "baz"
std::string Path::basename() const
{
    return RemoveExtension(filename());
}

// "/foo/bar/baz.txt" --> ".txt"
// "/foo/bar/baz" --> ""
std::string Path::extension() const
{
    std::string strFilename = filename();
    std::string::size_type n = strFilename.find_last_of('.');
    if (n != std::string::npos)
        return strFilename.substr(n);
    return "";
}

// "/foo/bar/baz.txt" --> "/foo/bar/"
Path Path::parent_path() const
{
    if (is_root())
        return get_root();

    if (is_normal_syntax())
        return Path(m_strPath.substr(0, m_strPath.find_last_of(separator)));
    return get_syntax_normalized().parent_path();
}


bool Path::exists() const
{
    const Path absp = is_absolute() ? Path(*this) : resolve_absolute();
#if defined(_POSIX_VERSION)
    return access(absp.string().c_str(), F_OK) != -1;
#elif defined(_WIN32)
    return _waccess(absp.wstring().c_str(), 0) != -1;
#endif
}

bool Path::is_root() const
{
    if (split().size() == 1) {
#if defined(_WIN32)
        // "x:" or "y:/" or ...
        if ((m_strPath.size() == 2 || m_strPath.size() == 3)
            && m_strPath[1] == colon) {
            return true;
        }
#endif
        // "//foo" for UNC paths
        if (m_strPath.size() > 2
            && m_strPath[0] == separator
            && m_strPath[1] == separator
            && m_strPath[2] != separator) {
            return true;
        }

        // "/foo" for normal UNIX paths
        if (m_strPath.size() > 1
            && m_strPath[0] == separator
            && m_strPath[1] != separator) {
            return true;
        }
    }

    // Just "/" or "//"
    if (m_strPath == "/" || m_strPath == "//")
        return true;
    return false;
}

bool Path::is_dir() const
{
#if defined(_POSIX_VERSION)
    struct stat sb;
    lstat(m_strPath.c_str(), &sb);
    return (sb.st_mode & S_IFMT) == S_IFDIR;
#elif defined(_WIN32)
    DWORD fileAttributes = GetFileAttributesW(this->wstring().c_str());
    return fileAttributes != INVALID_FILE_ATTRIBUTES
        && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#endif
}

bool Path::is_normal_syntax() const
{
    const std::vector<std::string> parts = this->split();

    return  count(parts.begin(), parts.end(), "..") == 0 &&
            count(parts.begin(), parts.end(), ".") == 0;
}

bool Path::is_absolute() const
{
#if defined(_WIN32)
    // "c:/..." or "d:/..." or ...
    if (m_strPath.size() > 2 && m_strPath[1] == colon)
        return true;
#endif
    // "//foo/bar" for UNC paths
    if (m_strPath.size() > 2
        && m_strPath[0] == separator
        && m_strPath[1] == separator
        && m_strPath[2] != separator) {
        return true;
    }

    // "/foo/bar" for normal UNIX paths
    if (m_strPath.size() > 1
        && m_strPath[0] == separator
        && m_strPath[1] != separator) {
        return true;
    }

    return false;
}

bool Path::empty() const
{
    return m_strPath.empty();
}

void Path::replace_extension(const std::string& new_extension) {
    std::string strFilename = filename();
    const Path parent = parent_path();

    const size_t dotPos = strFilename.find_last_of('.');
    if (dotPos != std::string::npos)
        strFilename.erase(dotPos);

    m_strPath = generic_to_native((parent / strFilename).string());

    if (new_extension.empty())
        return;
    m_strPath.append(new_extension);
    return;
}

Path Path::get_root() const
{
#if defined(_WIN32)
    // "c:" or "d:" or ...
    if (m_strPath.size() >= 2
        && m_strPath[1] == colon) {
        return m_strPath.substr(0, 2);
    }
#endif

    if (m_strPath.size() >= 1
        && m_strPath[0] == separator) {

        // "//foo" for UNC paths
        if (m_strPath.size() >= 2
            && m_strPath[1] == separator
            && m_strPath[2] != separator) {

            // "//"
            if ((m_strPath.size() >= 4 &&m_strPath.substr(0, 4) == "//..")
                || (m_strPath.size() >= 4 && m_strPath.substr(0, 4) == "//./")
                || (m_strPath.size() == 3 && m_strPath == "//.")
                || m_strPath == "//") {
                return Path("//");
            }

            // "//foo"
            const size_t endOfDriveName = m_strPath.find(separator, 2);
            return Path(m_strPath.substr(0, endOfDriveName));
        }

        // "/"
        if ((m_strPath.size() >= 3 && m_strPath.substr(0, 3) == "/..")
            || (m_strPath.size() >= 3 && m_strPath.substr(0, 3) == "/./")
            || (m_strPath.size() == 2 && m_strPath == "/.")
            || m_strPath == "/") {
            return Path("/");
        }

        // "/foo"
        return Path(m_strPath.substr(0, m_strPath.find(separator, 1)));
    }
    return Path();
}

/**
* @brief   Return the filenames of all files that have the specified extension
*          in the specified directory and all subdirectories (if needed).
* @param strPath the path of the directory
* @return a list of all the files paths in the specific directory
*/
Path::paths Path::get_all_files()
{
    paths directoryContents;

    if (!is_dir())
        return directoryContents;

    Path p = trim_trailing_slashes();

#if defined(_POSIX_VERSION)
    DIR* dir;
    struct dirent* file;

    dir = opendir(p.string().c_str());
    if (dir) {
        while ((file = readdir(dir)) != NULL) {
            directoryContents.emplace_back(p.string() + separator + file->d_name);
        }
        closedir(dir);
    }
#elif defined(_WIN32)
    WIN32_FIND_DATA findFileData;

    wchar_t preppedFilePath[MAX_PATH];
    wcsncpy_s(preppedFilePath, p.wstring().c_str(), MAX_PATH);
    wcsncat_s(preppedFilePath, L"\\*", 2);

    // On Windows we find the first file, then loop around for the others.
    HANDLE findHandle = FindFirstFile(preppedFilePath, &findFileData);

    if (findHandle == INVALID_HANDLE_VALUE) {
        return directoryContents;
    }

    directoryContents.emplace_back(p.wstring() + wide_separator + findFileData.cFileName);
    while (FindNextFile(findHandle, &findFileData) != 0) {
        directoryContents.emplace_back(p.wstring() + wide_separator + findFileData.cFileName);
    }
    FindClose(findHandle);
#endif

    return directoryContents;
}

/**
* @brief   Return the filenames of all files that have the specified extension
*          in the specified directory and all subdirectories (if needed).
* @param strPath the path of the directory
* @return a list of all the files paths in the specific directory
*/
Path::paths Path::get_all_files(const std::string& strExt)
{
    paths directoryContents = get_all_files();

    directoryContents.erase(std::remove_if(directoryContents.begin(), directoryContents.end(), [&strExt](const Path& path)
    {
        if (path.is_dir())
            return true;
        if (path.extension() != strExt)
            return true;
        return false;
    }), directoryContents.end());
    return directoryContents;
}

Path Path::resolve_absolute() const
{
#if defined(_POSIX_VERSION)
    std::unique_ptr<char, decltype(&std::free)> rpath(realpath(m_strPath.c_str(), NULL), std::free);
    if (!rpath) {
        return Path("");
    }
    return Path(rpath.get());
#elif defined(_WIN32)
    HANDLE hFile = CreateFile(this->wstring().c_str(),   // file to open
        GENERIC_READ,                          // open for reading
        FILE_SHARE_READ,                       // share for reading
        NULL,                                  // default security
        OPEN_EXISTING,                         // existing file only
        this->is_dir() ?
                                FILE_FLAG_BACKUP_SEMANTICS :       // directory
                                FILE_ATTRIBUTE_NORMAL,             // file
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Could not get handle: " + m_strPath);
    }

    // Query the length of the output string.
    DWORD pathLength = GetFinalPathNameByHandle(hFile,              // handle to file
        NULL,               // null buffer
        0,                  // max buffer size
        VOLUME_NAME_DOS);   // path with normal volume label

    if (pathLength == 0)
    {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to resolve path for: " + m_strPath);
    }

    std::unique_ptr<wchar_t[]> buffer = std::make_unique<wchar_t[]>(pathLength);
    DWORD result = GetFinalPathNameByHandle(hFile,             // handle to file
        buffer.get(),      // buffer that receives the resolved path
        pathLength,        // max buffer size

                    VOLUME_NAME_DOS);  // path with normal volume label
    CloseHandle(hFile);

    if (result == 0 || result > pathLength)
        throw std::runtime_error("Failed to resolve path for: " + this->string());

    // Need to lop "\\?\" off the front.
    return Path(buffer.get() + 4);
#endif
}

Path Path::get_syntax_normalized() const
{
    if (is_normal_syntax())
        return Path(*this);

    Path p;
    const std::vector<std::string> parts = split();
    for (const auto& part : parts) {
        // Resolve ".." by moving up a level and continuing.
        if (part == "..") {
            p = p.parent_path();
            continue;
        }

        // Resolve "." by not adding it in the first place and
        // continuing.
        if (part == ".")
            continue;

        p /= part;
    }

    return p;
}

std::vector<std::string> Path::split() const
{
    std::vector<std::string> vResult;

    if (m_strPath.empty())
        return vResult;

    const std::string strRoot = this->get_root().string();
    if (!strRoot.empty()) {
        vResult.push_back(strRoot);
    }

    size_t pos = std::string::npos, prev_pos = strRoot.size();
    while ((pos = m_strPath.find(separator, prev_pos)) != std::string::npos) {
        const std::string token = m_strPath.substr(prev_pos, pos - prev_pos);
        if (!token.empty())
            vResult.push_back(token);
        prev_pos = pos + 1;
    }

    // Get whatever was hanging if it isn't a set of "/"s.
    if (m_strPath.find(separator, prev_pos) == std::string::npos) {
        const std::string hanging = m_strPath.substr(prev_pos);
        if (!hanging.empty()) {
            vResult.push_back(hanging);
        }
    }
    return vResult;
}

Path Path::trim_trailing_slashes() const
{
    Path p(*this);

    // Don't let internal representations end in a trailing slash
    // (unless it's a path to the root directory) because it
    // complicates all further logic without gain.
    while (p.m_strPath.back() == separator
           && p.m_strPath.size() > 2) {
        p.m_strPath.pop_back();
    }

    return p;
}

Path& Path::operator/=(const Path& p)
{
    if (p.empty())
        return *this;

    if (p.m_strPath.front() != separator
        && !m_strPath.empty()
        && m_strPath.back() != separator) {
        m_strPath.push_back(separator);
    }

    m_strPath.append(p.m_strPath);
    return *this;
}

Path& Path::operator/=(const std::string& s) {
    return this->operator/=(Path(s));
}

Path operator/(const Path& lhs, const Path& rhs) {
    return Path(lhs) /= rhs;
}

Path operator/(const Path& lhs, const std::string& rhs) {
    return Path(lhs) /= Path(rhs);
}

Path operator/(const Path& lhs, const char* rhs) {
    return Path(lhs) /= Path(rhs);
}

bool operator==(const Path& lhs, const Path& rhs) {
    return lhs.get_syntax_normalized().string() == rhs.get_syntax_normalized().string();
}