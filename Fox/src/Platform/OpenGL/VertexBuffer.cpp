//
// Created by samuel on 26/06/2021.
//

#include <Core/Logger/Logger.hpp>
#include <glad/glad.h>
#include <OpenGL/OpenGLUtils.hpp>
#include "VertexBuffer.hpp"

namespace fox
{
    OpenGLVertexBuffer::OpenGLVertexBuffer()
        : m_oLayout(), m_Data(nullptr), m_InstanceElement(0), m_IsUploaded(false)
    {
        GLCall(glGenBuffers(1, &m_RendererID));
    }

    //OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) : OpenGLVertexBuffer()
    //{
    //    GLCall(glGenBuffers(1, &m_RendererID));
    //    
    //    // Bind the ID as a array
    //    Bind();

    //    // Fill the array
    //    GLCall(glBufferData(GL_ARRAY_BUFFER, size, m_Data, GL_DYNAMIC_DRAW));

    //    // Unbind
    //    Unbind();
    //    m_IsUploaded = true;
    //}

    //OpenGLVertexBuffer::OpenGLVertexBuffer(void *data, uint32_t size) : OpenGLVertexBuffer()
    //{
    //    m_Data = data;

    //    // Generate a new ID
    //    GLCall(glGenBuffers(1, &m_RendererID));

    //    // Bind the ID as a array
    //    Bind();

    //    // Fill the array
    //    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

    //    // Unbind
    //    Unbind();
    //    m_IsUploaded = true;
    //}

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLVertexBuffer::Initialize(size_t bufferSize, uint32_t numBuffers, bool dynamic, void* staticData)
    {
        // Ensure 4-byte alignment
        size_t alignedRangeSize = (bufferSize + 3) & ~3;
        m_DataSize = alignedRangeSize * numBuffers;

        m_Data = static_cast<uint8_t*>(staticData);

        // Bind the ID as a array
        Bind();

        // Fill the array
        GLCall(glBufferData(GL_ARRAY_BUFFER, m_DataSize, m_Data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        
        // Unbind
        Unbind();
        m_IsUploaded = true;
    }

    bool OpenGLVertexBuffer::BeginUpdate()
    {
        /*if (0 == m_vbo)
        {
            return false;
        }*/
        // Next buffer in the cycle
        //uint32_t  nextIndex = (m_index + 1) % m_numFrames;

        // If we are persistently mapped, then we don't need to map anything each frame
        /*if (!m_bPersistent)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            GLbitfield flags = GL_MAP_WRITE_BIT |
                GL_MAP_INVALIDATE_RANGE_BIT |
                GL_MAP_UNSYNCHRONIZED_BIT;
            GLvoid* pBuff = glMapBufferRange(GL_ARRAY_BUFFER, m_dataSize * nextIndex, m_dataSize, flags);
            m_vboData = static_cast<uint8_t*>(pBuff);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            if (nullptr == m_vboData)
            {
                return false;
            }
        }*/

        //m_index = nextIndex;
        return true;
    }

    void OpenGLVertexBuffer::EndUpdate()
    {
        /*if (m_bPersistent)
        {
            return;
        }

        if (nullptr != m_vboData)
        {
            if (0 != m_vbo)
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            m_vboData = nullptr;
        }*/
    }

    void OpenGLVertexBuffer::Bind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }

    void OpenGLVertexBuffer::Unbind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    const BufferLayout &OpenGLVertexBuffer::GetLayout() const
    {
        return m_oLayout;
    }

    void OpenGLVertexBuffer::SetLayout(const BufferLayout &layout, uint32_t instanceElement)
    {
        m_oLayout = layout;
        m_InstanceElement = instanceElement;
    }

    void OpenGLVertexBuffer::SetData(const void *data, uint32_t size)
    {
        Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
    }


    // -- POOL

    OpenGLVertexBufferPool::OpenGLVertexBufferPool()
        : m_oLayout(), m_Data(nullptr), m_IsUploaded(false)
    {
        GLCall(glGenBuffers(1, &m_RendererID));
    }

    OpenGLVertexBufferPool::~OpenGLVertexBufferPool()
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLVertexBufferPool::Initialize(size_t subRangeSize, uint32_t numSubRange, bool dynamic, void* staticData)
    {
        m_SubRangeSize = subRangeSize;
        m_numSubRanges = numSubRange;

        // Ensure 4-byte alignment
        m_DataSize = ((m_numSubRanges * m_SubRangeSize) + 3) & ~3;

        m_Data = static_cast<uint8_t*>(staticData);

        m_NumFrames = 1;
        m_BufferSize = m_DataSize * m_NumFrames;

        // Bind the ID as a array
        Bind();

        // Fill the array
        GLCall(glBufferData(GL_ARRAY_BUFFER, m_BufferSize, m_Data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

        // Unbind
        Unbind();
        m_IsUploaded = true;
    }

    bool OpenGLVertexBufferPool::BeginUpdate()
    {
        if (m_RendererID == 0)
        {
            return false;
        }
        // Next buffer in the cycle
        uint32_t  nextIndex = (m_Index + 1) % m_NumFrames;

        // If we are persistently mapped, then we don't need to map anything each frame
        /*if (!m_bPersistent)
        {*/
            Bind();
            GLbitfield flags = GL_MAP_WRITE_BIT |
                GL_MAP_INVALIDATE_RANGE_BIT |
                GL_MAP_UNSYNCHRONIZED_BIT;
            GLvoid* pBuff = glMapBufferRange(GL_ARRAY_BUFFER, m_DataSize * nextIndex, m_DataSize, flags);
            m_Data = static_cast<uint8_t*>(pBuff);
            Unbind();
            if (m_Data == nullptr)
            {
                return false;
            }
        //}

        m_Index = nextIndex;
        return true;
    }

    void OpenGLVertexBufferPool::EndUpdate()
    {
        /*if (m_bPersistent)
        {
            return;
        }*/

        if (nullptr != m_Data)
        {
            if (0 != m_RendererID)
            {
                Bind();
                glUnmapBuffer(GL_ARRAY_BUFFER);
                Unbind();
            }
            m_Data = nullptr;
        }
    }

    void OpenGLVertexBufferPool::Bind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }

    void OpenGLVertexBufferPool::Unbind() const
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    const BufferLayout& OpenGLVertexBufferPool::GetLayout() const
    {
        return m_oLayout;
    }

    void OpenGLVertexBufferPool::SetLayout(const BufferLayout& layout, uint32_t instanceElement)
    {
        m_oLayout = layout;
    }

    void OpenGLVertexBufferPool::SetData(const void* data, uint32_t size)
    {
        Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, GetDynamicOffset(), size, data));
    }

    void OpenGLVertexBufferPool::SetSubData(uint32_t subRangeIndex, const void* data, uint32_t size)
    {
        Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, GetDynamicOffset(subRangeIndex), size, data));
    }
}
