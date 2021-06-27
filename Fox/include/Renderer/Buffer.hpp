
//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_BUFFER_HPP
#define FOXENGINE_BUFFER_HPP

#include <Core/Assert.hpp>
#include "common.hpp"

namespace fox
{
    enum class ShaderDataType
    {
        None,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    static uint32_t ShaderDataTypeToSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:     return 4;
            case ShaderDataType::Float2:    return 4 * 2;
            case ShaderDataType::Float3:    return 4 * 3;
            case ShaderDataType::Float4:    return 4 * 4;
            case ShaderDataType::Mat3:      return 4 * 3 * 3;
            case ShaderDataType::Mat4:      return 4 * 4 * 4;
            case ShaderDataType::Int:       return 4;
            case ShaderDataType::Int2:      return 4 * 2;
            case ShaderDataType::Int3:      return 4 * 3;
            case ShaderDataType::Int4:      return 4 * 4;
            case ShaderDataType::Bool:      return 1;
            case ShaderDataType::None:
                break;
        }
        FOX_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct BufferElement
    {
        std::string m_strName;
        ShaderDataType m_eType;
        uint32_t m_uSize;
        uint32_t m_uOffset;
        bool m_bNormalized;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
                : m_strName(name), m_eType(type)
                , m_uSize(ShaderDataTypeToSize(type)), m_uOffset(0)
                , m_bNormalized(normalized)
        {
        }

        [[nodiscard]] uint32_t GetComponentCount() const
        {
            switch (m_eType)
            {
                case ShaderDataType::Float:     return 1;
                case ShaderDataType::Float2:    return 2;
                case ShaderDataType::Float3:    return 3;
                case ShaderDataType::Float4:    return 4;
                case ShaderDataType::Mat3:      return 3 * 3;
                case ShaderDataType::Mat4:      return 4 * 4;
                case ShaderDataType::Int:       return 1;
                case ShaderDataType::Int2:      return 2;
                case ShaderDataType::Int3:      return 3;
                case ShaderDataType::Int4:      return 4;
                case ShaderDataType::Bool:      return 1;
            }
            FOX_CORE_ASSERT(false, "Unknown ShaderDataType!");
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout();
        BufferLayout(const std::initializer_list<BufferElement>& elements);
        ~BufferLayout() = default;
        [[nodiscard]] const std::vector<BufferElement>& GetElements() const&;
        [[nodiscard]] uint32_t GetStride() const;

        std::vector<BufferElement>::iterator begin() { return  m_vElements.begin(); }
        std::vector<BufferElement>::iterator end() { return  m_vElements.end(); }

        [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return  m_vElements.begin(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return  m_vElements.end(); }

    private:
        void CalculateOffsetAndStride();

        std::vector<BufferElement> m_vElements;
        uint32_t m_iStride;
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() {};
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual unsigned int GetCount() const = 0;

        static ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);

    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() {};
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout) = 0;

        static ref<VertexBuffer> Create(float* vertices, uint32_t size);
    };
}

#endif //FOXENGINE_BUFFER_HPP
