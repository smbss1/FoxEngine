//
// Created by samuel on 02/10/22.
//

#ifndef FOX_ENGINE_UUID_HPP
#define FOX_ENGINE_UUID_HPP

#include <cstddef>
#include <cstdint>

namespace fox
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };
}

namespace std {
    template <typename T> struct hash;

    template<>
    struct hash<fox::UUID>
    {
        std::size_t operator()(const fox::UUID& uuid) const
        {
            return (uint64_t)uuid;
        }
    };

}


#endif //FOX_LEXER_UUID_HPP
