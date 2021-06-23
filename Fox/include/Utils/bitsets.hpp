
#ifndef FOX_BITSET_HPP_
#define FOX_BITSET_HPP_

#include <iostream>
#include <cstring>
#include "Core/Core.hpp"

class bitset
{
private:
    char* data;
    std::size_t m_lSize;

public:
    bitset() : m_lSize(8)
    {
        data = new char[m_lSize + 1];
        if (data)
            reset();
    }

    bitset(std::size_t s) : m_lSize(s)
    {
        data = new char[m_lSize + 1];
        if (data)
            reset();
    }

    bitset(const bitset& other)
    {
        m_lSize = other.m_lSize;
        data = new char[m_lSize + 1];
        std::memcpy(data, other.data, m_lSize);
    }

    ~bitset()
    {
        if (data) {
            m_lSize = 0;
            delete[] data;
            data = nullptr;
        }
    }

    bitset& operator=(const bitset& other)
    {
        m_lSize = other.m_lSize;
        data = nullptr;
        data = new char[m_lSize + 1];
        std::memcpy(data, other.data, m_lSize);
        return *this;
    }

    const char* getData() const;

    std::size_t size() const;
    std::size_t count() const;
    bool test(std::size_t n);
    bool none();
    bool any();
    void set();
    void set(std::size_t n, bool v);
    void reset();
    void reset(std::size_t n);
    void flip(std::size_t n);

    char &operator[](const std::size_t &n);	
    char &operator[](const std::size_t &n) const;	
    bool operator==(const bitset& other);	
};
bitset operator&(const bitset& lhs, const bitset& rhs);
std::ostream& operator<<(std::ostream& os, const bitset &b);
#endif