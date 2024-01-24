
#ifndef FOX_MEMORY_BUFFER
#define FOX_MEMORY_BUFFER

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "glm/glm.hpp"

namespace fox
{
    // template <typename POD>
    // class SerializablePOD
    // {
    // public:
    //     static size_t serialize_size(POD str)
    //     {
    //         return sizeof(POD);
    //     }
    //     static char* serialize(char* target, POD value)
    //     {
    //         return memcpy(target, &value, serialize_size(value));
    //     }
    //     static const char* deserialize(const char* source, POD& target)
    //     {
    //         memcpy(&target, source, serialize_size(target));
    //         return source + serialize_size(target);
    //     }
    // };

    // template<>
    // size_t SerializablePOD<char*>::serialize_size(char* str)
    // {
    //     return sizeof(size_t) + std::strlen(str);
    // }

    // template<>
    // const char* SerializablePOD<char*>::deserialize(const char* source, char*& target)
    // {
    //     size_t length;
    //     std::memcpy(&length, source, sizeof(size_t));
    //     std::memcpy(&target, source + sizeof(size_t), length);
    //     return source + sizeof(size_t) + length;
    // }

    class MemoryBuffer
    {
    public:
        MemoryBuffer()
        {
        }

        MemoryBuffer(size_t size)
        {
            allocateBiggerMemory(size);
        }

        MemoryBuffer(MemoryBuffer&& other)
        {
            delete[] data;
            data = std::exchange(other.data, nullptr);
            offset = std::exchange(other.offset, 0);
            bufferCapacity = std::exchange(other.bufferCapacity, 0);
        }

        MemoryBuffer(const MemoryBuffer& other)
        {
            if (bufferCapacity != other.bufferCapacity)           // resource in *this cannot be reused
            {
                char* temp = new char[other.bufferCapacity];   // allocate resource, if throws, do nothing
                delete[] data;              // release resource in *this
                data = temp;
                bufferCapacity = other.bufferCapacity;
            }

            offset = other.offset;
            bufferCapacity = other.bufferCapacity;

            std::memcpy(data, other.data, bufferCapacity);
        }

        ~MemoryBuffer()
        {
            if (data)
                delete[] data;
        }

        template <typename T>
        T read()
        {
            if (offset + sizeof(T) > bufferCapacity || offset < 0)
                throw std::length_error("Can't read out of bounds");

            offset -= sizeof(T);
            return *reinterpret_cast<T*>(data + offset);
        }

        template <typename T>
        T* read(size_t count)
        {
            if (offset + (count * sizeof(T)) > bufferCapacity || offset < 0 || (offset - (count * sizeof(T))) < 0)
                throw std::length_error("Can't read out of bounds");

            offset -= count * sizeof(T);
            return reinterpret_cast<T*>(data + offset);
        }

        template <typename T>
        void write(const T& value)
        {
            write<T>(&value, sizeof(T));
        }

        template <typename T>
        void write(T& value)
        {
            write<T>(&value, sizeof(T));
        }

        //template <typename T>
        //void write(const T* value)
        //{
        //    /*std::size_t offset = (_offset == SIZE_MAX) ? bufferLength : _offset;

        //    if (offset < 0)
        //        throw std::length_error("Can't write to a negative offset");*/

        //    if (offset + sizeof(T) >= bufferCapacity)
        //        allocateBiggerMemory<T>();

        //    std::memcpy(data + offset, value, sizeof(T));
        //    offset += sizeof(T);
        //}

        template <typename T>
        void write(const T* value, size_t size)
        {
            write((T*)value, size);
        }

        template <typename T>
        void write(T* value, size_t size)
        {
            /*if (offset + (count * sizeof(T)) >= bufferCapacity)
                allocateBiggerMemory(count * sizeof(T));

            std::memcpy(data + offset, value, count * sizeof(T));
            offset += count * sizeof(T);*/

            if (offset + size >= bufferCapacity)
                allocateBiggerMemory(size);

            std::memcpy(data + offset, value, size);
            offset += size;
        }

        void clear()
        {
            //std::memset(data, 0, bufferCapacity);
            offset = 0;
        }

        void reset()
        {
            if (data)
                delete[] data;
            data = nullptr;
            offset = 0;
            bufferCapacity = 0;
        }

        void append(const MemoryBuffer& buffer)
        {
            if (offset + buffer.size() >= bufferCapacity)
                allocateBiggerMemory(offset + buffer.size());

            std::memcpy(data + offset, buffer.buffer(), buffer.size());
            offset += buffer.size();
        }

        void append(void* buffer, size_t size)
        {
            if (offset + size >= bufferCapacity)
                allocateBiggerMemory(offset + size);

            std::memcpy(data + offset, buffer, size);
            offset += size;
        }

        std::size_t capacity() const { return bufferCapacity; }
        std::size_t size() const { return offset; }
        char* buffer() const { return data; }
        char** bufferMutable() { return &data; }

        MemoryBuffer& operator=(const MemoryBuffer& other) noexcept // call copy or move constructor to construct other
        {
            if (this == &other)
                return *this;

            if (bufferCapacity != other.bufferCapacity)           // resource in *this cannot be reused
            {
                char* temp = new char[other.bufferCapacity];   // allocate resource, if throws, do nothing
                delete[] data;              // release resource in *this
                data = temp;
                bufferCapacity = other.bufferCapacity;
            }
            offset = other.offset;
            bufferCapacity = other.bufferCapacity;

            std::memcpy(data, other.data, bufferCapacity);
            return *this;
        }

        MemoryBuffer& operator=(MemoryBuffer&& other) noexcept // call copy or move constructor to construct other
        {
            if (this == &other)
                return *this;

            delete[] data;
            data = std::exchange(other.data, nullptr);
            offset = std::exchange(other.offset, 0);
            bufferCapacity = std::exchange(other.bufferCapacity, 0);
            return *this;
        }

    private:

        void allocateBiggerMemory(size_t size)
        {
            // allocate new & bigger memory
            size_t newSize = (bufferCapacity + size);
            char* new_buffer = new char[newSize];
            std::memset(new_buffer, 0, newSize);

            if (data) {
                std::memcpy(new_buffer, data, bufferCapacity);
                delete[] data;
            }
            data = new_buffer;
            bufferCapacity = newSize;
        }

        char* data = nullptr;
        uint32_t offset = 0;
        uint32_t bufferCapacity = 0;
    };
}

#endif