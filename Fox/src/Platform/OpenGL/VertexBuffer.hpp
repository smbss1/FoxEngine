//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_VERTEXBUFFER_HPP
#define FOXENGINE_VERTEXBUFFER_HPP

#include "Renderer/VertexBuffer.hpp"

namespace fox
{
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer();
        //explicit OpenGLVertexBuffer(uint32_t size);
        //OpenGLVertexBuffer(void *data, uint32_t size);

        ~OpenGLVertexBuffer() override;
        void Initialize(size_t bufferSize, uint32_t numBuffers = 1, bool dynamic = false, void* staticData = nullptr) override;
        bool IsUploaded() const override { return m_IsUploaded; }
        virtual uint32_t GetMaxInstanceCount() const override { return m_InstanceElement; }

        void Bind() const override;
        void Unbind() const override;

        bool BeginUpdate() override;
        void EndUpdate() override;

        [[nodiscard]] const BufferLayout& GetLayout() const override;
        void SetLayout(const BufferLayout& layout, uint32_t instanceElement = 1) override;

        void SetData(const void* data, uint32_t size) override;

        /// Returns the pointer to the currently active buffer data in the shared VBO
        /// \return Pointer to the current writeable (possibly a sub-range of the) vertex buffer
        uint8_t* GetData() override
        {
            if (m_Data == nullptr)
            {
                return nullptr;
            }
            //if (m_bPersistent)
            //{
            //    // m_vboData always points to the beginning of the buffer, so we must
            //    // account for the offset of the currently used section
            //    return m_Data + (m_index * m_dataSize);
            //}
            //else
            //{
                // m_vboData always points to the beginning of the currently mapped
                // range of the buffer, so no additional offset is used
            return m_Data;
            //}
        }

        /// Returns the current offset into the VBO to start rendering from
        uint32_t GetDynamicOffset() override { return 0; }

        /// Returns the GL "Name" of the currently active VBO
        uint32_t GetID() override { return m_RendererID; }

    private:
        unsigned int m_RendererID{};
        BufferLayout m_oLayout{};
        uint8_t* m_Data = nullptr;
        bool m_IsUploaded{};

        uint32_t m_InstanceElement{0};
    };


    class OpenGLVertexBufferPool : public VertexBufferPool
    {
    public:
        OpenGLVertexBufferPool();
        ~OpenGLVertexBufferPool() override;

        void Initialize(size_t bufferSize, uint32_t numBuffers = 1, bool dynamic = false, void* staticData = nullptr) override;
        bool IsUploaded() const override { return m_IsUploaded; }
        virtual uint32_t GetMaxInstanceCount() const override { return 0; }

        void Bind() const override;
        void Unbind() const override;

        [[nodiscard]] const BufferLayout& GetLayout() const override;
        void SetLayout(const BufferLayout& layout, uint32_t instanceElement = 1) override;

        /// Returns the pointer to the currently active buffer data in the shared VBO
        /// \return Pointer to the current writeable (possibly a sub-range of the) vertex buffer
        uint8_t* GetData() override
        {
            if (m_Data == nullptr)
            {
                return nullptr;
            }
            //if (m_bPersistent)
            //{
            //    // m_vboData always points to the beginning of the buffer, so we must
            //    // account for the offset of the currently used section
            //    return m_Data + (m_index * m_dataSize);
            //}
            //else
            //{
                // m_vboData always points to the beginning of the currently mapped
                // range of the buffer, so no additional offset is used
                return m_Data;
            //}
        }

        uint8_t* GetData(uint32_t subRangeIndex)
        {
            if (m_Data == nullptr)
            {
                return nullptr;
            }
            //if (m_bPersistent)
            //{
            //    // m_Data always points to the beginning of the buffer, so we must
            //    // account for the offset of the currently used section
            //    return m_Data + (m_Index * m_DataSize) + (subRangeIndex * m_SubRangeSize);
            //}
            //else
            //{
                // m_Data always points to the beginning of the currently mapped
                // range of the buffer, so no additional per-frame offset is used
                return m_Data + (subRangeIndex * m_SubRangeSize);
            //}
        }

        /// Returns the offset to the currently active section of the VBO
        /// \return Offset, in bytes, from the beginning of the VBO to the currently
        ///         active frame's section
        uint32_t GetDynamicOffset() override { return m_Index * m_DataSize; } // or index * m_DataSize

        /// Returns the offset into the shared VBO of the sub-range with the 
        /// provided index
        /// \param subRangeIndex Index of the sub-range for which to retrieve the offset
        /// \return Offset, in bytes, from the start of the vertex buffer at which the
        ///         sub-range with the given index begins.
        uint32_t GetDynamicOffset(uint32_t subRangeIndex) override { return GetDynamicOffset() + (subRangeIndex * m_SubRangeSize); }

        /// Returns the GL "Name" of the currently active VBO
        uint32_t GetID() override { return m_RendererID; }

        void SetData(const void* data, uint32_t size) override;
        void SetSubData(uint32_t subRangeIndex, const void* data, uint32_t size) override;

        bool BeginUpdate() override;
        void EndUpdate() override;

    private:
        unsigned int m_RendererID{};
        BufferLayout m_oLayout{};
        uint8_t* m_Data = nullptr;
        bool m_IsUploaded{};

        uint32_t    m_Index = 0;
        uint32_t    m_NumFrames = 0;
        uint32_t    m_BufferSize = 0;
        uint32_t    m_numSubRanges = 0;
        uint32_t    m_SubRangeSize = 0;
    };
}

#endif //FOXENGINE_VERTEXBUFFER_HPP
