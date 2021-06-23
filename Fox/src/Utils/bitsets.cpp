/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** bitsets.cpp
*/

#include "Utils/bitsets.hpp"

const char* bitset::getData() const
{
    return data;
}

std::size_t bitset::count() const
{
    std::size_t c = 0;
    for (int i = 0; i < m_lSize; i++)
    {
        c += (int)data[i];
    }
    return c;
}

bool bitset::test(std::size_t n)
{
    return data[n] == '1';
}

bool bitset::none()
{
    for (int i = 0; i < m_lSize; i++)
    {
        if (data[i] == '1')
        {
            return 0;
        }
    }
    return 1;
}

bool bitset::any()
{
    for (int i = 0; i < m_lSize; i++)
    {
        if (data[i] == '1')
        {
            return 1;
        }
    }
    return 0;
}

std::size_t bitset::size() const
{
    return m_lSize;
}

void bitset::set()
{
    std::fill(data, data + m_lSize, '1');
}

void bitset::set(std::size_t n, bool v)
{
    if (n > m_lSize) {
        char* new_data = new char[n + 1]();
        if (new_data) {
            new_data[n] = '\0';
            std::fill(new_data, new_data + n, '0');
            std::memcpy(new_data, data, m_lSize);
            if (data)
                delete data;
            data = new_data;
            m_lSize = n;
        }
    }
    data[n] = v ? '1' : '0';
}

void bitset::reset()
{
    std::fill(data, data + m_lSize, '0');
    data[m_lSize] = '\0';
}

void bitset::reset(std::size_t n)
{
    data[n] = '0';
}

void bitset::flip(std::size_t n)
{
    data[n] = (data[n] == '0') ? '1' : '0';
}

char &bitset::operator[](const std::size_t &n)
{
    return data[n];
}

char &bitset::operator[](const std::size_t &n) const
{
    return data[n];
}
bool bitset::operator==(const bitset& other)
{
    bool res = true;
    std::size_t min = std::min(size(), other.size());
    for (int i = 0; i < min; ++i) {
        if (data[i] != other[i]) {
            res = false;
        }
    }
    return res;
}

bitset operator&(const bitset& lhs, const bitset& rhs)
{
    std::size_t min = std::min(lhs.size(), rhs.size());
    bitset newBit(min);
    for (int i = 0; i < min; ++i) {
        if (lhs[i] == '1' && rhs[i] == '1')
            newBit.set(i, true);
    }
    return newBit;
}

std::ostream& operator<<(std::ostream& os, const bitset &b)
{
    os << b.getData();
    return os;
}