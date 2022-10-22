//
// Created by samuel on 22/10/22.
//

#ifndef FOX_LEXER_STRING_HPP
#define FOX_LEXER_STRING_HPP

namespace fox::Utils::String
{
    std::vector<std::string> SplitString(const std::string_view string, const std::string_view &delimiters);
    std::vector<std::string> SplitString(const std::string_view string, const char delimiter);

    std::string& ToLower(std::string& string);
    std::string ToLowerCopy(const std::string_view string);

}

#endif //FOX_LEXER_STRING_HPP
