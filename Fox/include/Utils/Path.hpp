/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** Path.hpp header
*/

#ifndef ECS_PATH_HPP
#define ECS_PATH_HPP

#include <vector>
#include <string>

class Path
{
public:
    using paths = std::vector<Path>;
    Path() {}
    Path(const std::string& s);
    Path(const std::wstring& s);
    Path(const char* s);
    Path(const wchar_t* s);
    ~Path() {}

    std::string string() const;
    std::wstring wstring() const;
    std::string filename() const;
    std::string basename() const;
    std::string extension() const;
    Path parent_path() const;

    bool exists() const;
    bool is_root() const;
    bool is_dir() const;
    bool is_normal_syntax() const;
    bool is_absolute() const;
    bool empty() const;

    void replace_extension(const std::string& new_extension);
    Path get_root() const;
    paths get_all_files();
    paths get_all_files(const std::string& strExt);
    Path resolve_absolute() const;
    Path get_syntax_normalized() const;

    std::vector<std::string> split() const;
    Path trim_trailing_slashes() const;

    Path& operator/=(const std::string& s);
    Path& operator/=(const Path& p);
private:
    std::string m_strPath;
};

Path operator/(const Path& lhs, const Path& rhs);
Path operator/(const Path& lhs, const std::string& rhs);
Path operator/(const Path& lhs, const char* rhs);
bool operator==(const Path& lhs, const Path& rhs);

#endif //ECS_PATH_HPP
