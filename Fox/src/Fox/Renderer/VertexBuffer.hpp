
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
        ShaderDataType m_eType;
        uint32_t m_uIndex;
        uint32_t m_uSize;
        uint32_t m_uOffset;
        uint32_t m_uDivisor;
        bool m_bNormalized;

        BufferElement(uint32_t index, ShaderDataType type, const std::string& name, uint32_t divisor = 0, bool normalized = false)
            : m_eType(type)
            , m_uSize(ShaderDataTypeToSize(type)), m_uOffset(0)
            , m_bNormalized(normalized)
            , m_uIndex(index)
            , m_uDivisor(divisor)
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
                case ShaderDataType::Mat3:      return 3;
                case ShaderDataType::Mat4:      return 4;
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

    // Currently Fox only supports 32-bit index buffers
    class IndexBuffer : public RefCounted
    {
    public:
        virtual ~IndexBuffer() {};
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual unsigned int GetCount() const = 0;

        static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
    };

    class VertexBuffer : public RefCounted
    {
    protected:
        uint32_t m_DataSize;

    public:
        virtual ~VertexBuffer() {};
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout, uint32_t instanceElement = 1) = 0;

        virtual void Initialize(size_t bufferSize, uint32_t numBuffers = 1, bool dynamic = false, void* staticData = nullptr) = 0;
        virtual bool IsUploaded() const = 0;

        virtual uint32_t GetMaxInstanceCount() const = 0;

        virtual void SetData(const void *data, uint32_t size) = 0;
        /// Returns the pointer to the currently active buffer data in the shared VBO
        /// \return Pointer to the current writeable (possibly a sub-range of the) vertex buffer
        virtual uint8_t* GetData() = 0;

        virtual bool BeginUpdate() = 0;
        virtual void EndUpdate() = 0;

        /// Returns the size of each of the vertex buffers in the shared VBO
        /// \return Number of bytes per vertex buffer in the shared VBO
        virtual uint32_t GetDataSize() const { return m_DataSize; }

        /// Returns the current offset into the VBO to start rendering from
        virtual uint32_t GetDynamicOffset() = 0;

        /// Returns the GL "Name" of the currently active VBO
        virtual uint32_t GetID() = 0;

        static Ref<VertexBuffer> Create();
        //static Ref<VertexBuffer> Create(uint32_t size);
        //static Ref<VertexBuffer> Create(void* vertices, uint32_t size);
    };
    
    class VertexBufferPool : public VertexBuffer
    {
    public:
        virtual ~VertexBufferPool() {};

        virtual void SetSubData(uint32_t subRangeIndex, const void* data, uint32_t size) = 0;

        /// Returns the current offset into the VBO to start rendering from
        virtual uint32_t GetDynamicOffset(uint32_t subRangeIndex) = 0;

        static Ref<VertexBufferPool> Create();
    };
}

#endif //FOXENGINE_BUFFER_HPP
