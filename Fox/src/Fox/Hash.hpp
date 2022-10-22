//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_HASHES_HPP
#define FOXENGINE_HASHES_HPP

#include <cstdint>
#include <string>

namespace fox::Hash
{
    inline std::uint32_t fnv1a_32(char const* s, std::size_t count)
    {
        return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
    }

    inline std::uint32_t FNVHash(const std::string& s)
    {
        return fnv1a_32(s.c_str(), s.size());
    }
}

#endif //FOXENGINE_HASHES_HPP
