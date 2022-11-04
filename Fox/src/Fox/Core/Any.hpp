//
// Created by samuel on 02/11/22.
//

#ifndef FOX_LEXER_ANY_HPP
#define FOX_LEXER_ANY_HPP

#include "Core/Base.hpp"
#include "Core/Logger/Logger.hpp"
#include "Assert.hpp"
#include "Scripting/ValueWrapper.hpp"

namespace fox
{
    struct Any
    {
        void* Data;
        uint32_t Size;

        Any()
            : Data(nullptr), Size(0)
        {
        }

        Any(void* data, uint32_t size)
            : Data(data), Size(size)
        {
        }

        static Any Copy(const Any& other)
        {
            Any buffer;
            buffer.Allocate(other.Size);
            memcpy(buffer.Data, other.Data, other.Size);
            return buffer;
        }

        static Any Copy(const Utils::ValueWrapper& other)
        {
            Any buffer;
            buffer.Allocate(other.GetDataSize());
            memcpy(buffer.Data, other.GetRawData(), other.GetDataSize());
            return buffer;
        }

        static Any Copy(const void* data, uint32_t size)
        {
            Any buffer;
            buffer.Allocate(size);
            memcpy(buffer.Data, data, size);
            return buffer;
        }

        void Allocate(uint32_t size)
        {
            delete[] (byte*)Data;
            Data = nullptr;

            if (size == 0)
                return;

            Data = new byte[size];
            Size = size;
        }

        void Release()
        {
            delete[] (byte*)Data;
            Data = nullptr;
            Size = 0;
        }

        void ZeroInitialize()
        {
            if (Data)
                memset(Data, 0, Size);
        }

        template<typename T>
        T& Read(uint32_t offset = 0)
        {
            return *(T*)((byte*)Data + offset);
        }

        template<typename T>
        const T& Read(uint32_t offset = 0) const
        {
            return *(T*)((byte*)Data + offset);
        }

        byte* ReadBytes(uint32_t size, uint32_t offset)
        {
            FOX_CORE_ASSERT(offset + size <= Size, "Any overflow!");
            byte* buffer = new byte[size];
            memcpy(buffer, (byte*)Data + offset, size);
            return buffer;
        }

        void Write(const void* data, uint32_t size, uint32_t offset = 0)
        {
            FOX_CORE_ASSERT(offset + size <= Size, "Any overflow!");
            memcpy((byte*)Data + offset, data, size);
        }

        operator bool() const
        {
            return Data;
        }

        byte& operator[](int index)
        {
            return ((byte*)Data)[index];
        }

        byte operator[](int index) const
        {
            return ((byte*)Data)[index];
        }

        template<typename T>
        T* As() const
        {
            return (T*)Data;
        }

        inline uint32_t GetSize() const { return Size; }
    };

    struct AnySafe : public Any
    {
        ~AnySafe()
        {
            Release();
        }

        static AnySafe Copy(const void* data, uint32_t size)
        {
            AnySafe buffer;
            buffer.Allocate(size);
            memcpy(buffer.Data, data, size);
            return buffer;
        }
    };
}


#endif //FOX_LEXER_ANY_HPP
