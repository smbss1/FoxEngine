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
    struct AnyValue
    {
        void* Data = nullptr;
        uint32_t Size = 0;

        AnyValue()
            : Data(nullptr), Size(0)
        {
        }

        AnyValue(AnyValue&& other) : Data(other.Data), Size(other.Size)
        {
            other.Data = nullptr;
            other.Size = 0;
        }

        //template <typename T, typename U = typename std::remove_cv<std::remove_reference_t<std::decay_t<T>>>::type, typename = typename std::enable_if<!std::is_same_v<U, AnyValue>>::type>
        //AnyValue(T&& value)
        //{
        //    Allocate(sizeof(T));
        //    memcpy(Data, &value, sizeof(T));
        //}

        AnyValue(const AnyValue& other)
        {
            Allocate(other.Size);
            memcpy(Data, other.Data, other.Size);
        }

        AnyValue(int8_t value)
        {
            Allocate(sizeof(int8_t));
            memcpy(Data, &value, sizeof(int8_t));
        }

   /*     AnyValue(void* data, uint32_t size)
        {
            Allocate(size);
            memcpy(Data, data, size);
        }*/

        ~AnyValue()
        {
            Release();
        }

        AnyValue& operator=(AnyValue&& other)
        {
            // Guard self assignment
            if (this == &other)
                return *this;

            Data = other.Data;
            Size = other.Size;
            other.Data = nullptr;
            other.Size = 0;
            return *this;
        }

        static AnyValue Copy(const AnyValue& other)
        {
            AnyValue buffer;
            buffer.Allocate(other.Size);
            memcpy(buffer.Data, other.Data, other.Size);
            return buffer;
        }

        static AnyValue Copy(const Utils::ValueWrapper& other)
        {
            AnyValue buffer;
            buffer.Allocate(other.GetDataSize());
            memcpy(buffer.Data, other.GetRawData(), other.GetDataSize());
            return buffer;
        }

        static AnyValue Copy(const void* data, uint32_t size)
        {
            AnyValue buffer;
            buffer.Allocate(size);
            memcpy(buffer.Data, data, size);
            return buffer;
        }

        void Allocate(uint32_t size)
        {
            if (Data != nullptr)
            {
                delete[](byte*)Data;
                Data = nullptr;
            }

            if (size == 0)
                return;

            Data = new byte[size];
            Size = size;

            ZeroInitialize();
        }

        void Release()
        {
            delete[](byte*)Data;
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
            FOX_CORE_ASSERT(offset + size <= Size, "AnyValue overflow!");
            byte* buffer = new byte[size];
            memcpy(buffer, (byte*)Data + offset, size);
            return buffer;
        }

        void Write(const void* data, uint32_t size, uint32_t offset = 0)
        {
            FOX_CORE_ASSERT(offset + size <= Size, "AnyValue overflow!");
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

        template<typename T>
        T& AsRef() const
        {
            return *static_cast<T*>(Data);
        }

        void* Raw() const
        {
            return Data;
        }

        inline uint32_t GetSize() const { return Size; }
    };

    struct AnyValueSafe : public AnyValue
    {
        AnyValueSafe() : AnyValue()
        {
        }

        ~AnyValueSafe()
        {
            Release();
        }

        AnyValueSafe(AnyValueSafe&& other)
        {
            Data = std::move(other.Data);
            Size = std::move(other.Size);
        }

        static AnyValueSafe Copy(const void* data, uint32_t size)
        {
            AnyValueSafe buffer;
            buffer.Allocate(size);
            memcpy(buffer.Data, data, size);
            return buffer;
        }
    };
}


#endif //FOX_LEXER_ANY_HPP
