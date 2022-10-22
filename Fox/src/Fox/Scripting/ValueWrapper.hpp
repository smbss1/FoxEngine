//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_VALUEWRAPPER_HPP
#define FOXENGINE_VALUEWRAPPER_HPP

#include <filesystem>
#include <glm/glm.hpp>

#include <string>
#include <typeinfo>
#include <cstring>
#include "Core/Assert.hpp"

namespace fox::Utils
{
    using byte = uint8_t;

    class ValueWrapper
    {
    public:
        ValueWrapper() = default;

        ValueWrapper(int8_t value) : ValueWrapper(&value, sizeof(int8_t)) {}
        ValueWrapper(int16_t value) : ValueWrapper(&value, sizeof(int16_t)) {}
        ValueWrapper(int32_t value) : ValueWrapper(&value, sizeof(int32_t)) {}
        ValueWrapper(int64_t value) : ValueWrapper(&value, sizeof(int64_t)) {}
        ValueWrapper(uint8_t value) : ValueWrapper(&value, sizeof(uint8_t)) {}
        ValueWrapper(uint16_t value) : ValueWrapper(&value, sizeof(uint16_t)) {}
        ValueWrapper(uint32_t value) : ValueWrapper(&value, sizeof(uint32_t)) {}
        ValueWrapper(uint64_t value) : ValueWrapper(&value, sizeof(uint64_t)) {}
        ValueWrapper(float value) : ValueWrapper(&value, sizeof(float)) {}
        ValueWrapper(double value) : ValueWrapper(&value, sizeof(double)) {}
        ValueWrapper(const std::string& value) : ValueWrapper((void*)value.data(), value.size()) {}
        ValueWrapper(const glm::vec2& value) : ValueWrapper((void*)&value, sizeof(glm::vec2)) {}
        ValueWrapper(const glm::vec3& value) : ValueWrapper((void*)&value, sizeof(glm::vec3)) {}
        ValueWrapper(const glm::vec4& value) : ValueWrapper((void*)&value, sizeof(glm::vec4)) {}

        ValueWrapper(void* data, size_t size)
            : m_Size(size)
        {
            m_ValueBuffer = new byte[size];
            memcpy(m_ValueBuffer, data, size);
        }

        ValueWrapper(const ValueWrapper& other)
            : m_Size(other.m_Size)
        {
            m_ValueBuffer = new byte[other.m_Size];
            memcpy(m_ValueBuffer, other.m_ValueBuffer, other.m_Size);
        }

        ValueWrapper& operator=(const ValueWrapper& other)
        {
            m_Size = other.m_Size;
            m_ValueBuffer = new byte[m_Size];
            memcpy(m_ValueBuffer, other.m_ValueBuffer, m_Size);
            return *this;
        }

        ~ValueWrapper()
        {
            if (m_ValueBuffer)
            {
                m_Size = 0;
                delete[] m_ValueBuffer;
                m_ValueBuffer = nullptr;
            }
        }

        void ReleaseBuffer()
        {
            if (m_ValueBuffer)
            {
                m_Size = 0;
                delete[] m_ValueBuffer;
                m_ValueBuffer = nullptr;
            }
        }

        bool HasValue() const { return m_Size > 0 && m_ValueBuffer != nullptr; }

        template<typename TValueType>
        TValueType Get() const
        {
            FOX_CORE_ASSERT(HasValue(), "ValueWrapper::Get - No value!");

            if constexpr (std::is_same<TValueType, std::string>::value)
                return TValueType((char*)m_ValueBuffer, m_Size);
            else
                return *(TValueType*)m_ValueBuffer;
        }

        template<typename TValueType>
        TValueType GetOrDefault(const TValueType& defaultValue) const
        {
            if (!HasValue())
                return defaultValue;

            if constexpr (std::is_same<TValueType, std::string>::value)
                return TValueType((char*)m_ValueBuffer, m_Size);
            else
                return *(TValueType*)m_ValueBuffer;
        }

        void* GetRawData() const { return m_ValueBuffer; }
        size_t GetDataSize() const { return m_Size; }

        static ValueWrapper OfSize(size_t size)
        {
            ValueWrapper value;
            value.m_ValueBuffer = new byte[size];
            value.m_Size = size;
            return value;
        }

    private:
        size_t m_Size = 0;
        byte* m_ValueBuffer = nullptr;
    };
}

#endif //FOXENGINE_VALUEWRAPPER_HPP
