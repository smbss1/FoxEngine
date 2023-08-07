//
// Created by samuel on 27/06/2021.
//

#include "RendererCommand.hpp"

namespace fox
{
    // -- RenderCommandQueue
    RenderCommandQueue::RenderCommandQueue()
    {
        m_CommandBuffer = new uint8_t[10 * 1024 * 1024]; // 10mb buffer
        m_CommandBufferPtr = m_CommandBuffer;
        memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
    }

    RenderCommandQueue::~RenderCommandQueue()
    {
        delete[] m_CommandBuffer;
    }

    void* RenderCommandQueue::Push(RenderCommandFn fn, uint32_t size)
    {
        // TODO: alignment
        *(RenderCommandFn*)m_CommandBufferPtr = fn;
        m_CommandBufferPtr += sizeof(RenderCommandFn);

        *(uint32_t*)m_CommandBufferPtr = size;
        m_CommandBufferPtr += sizeof(uint32_t);

        void* memory = m_CommandBufferPtr;
        m_CommandBufferPtr += size;

        m_CommandCount++;
        return memory;
    }

    void RenderCommandQueue::Execute()
    {
        byte* buffer = m_CommandBuffer;

        for (uint32_t i = 0; i < m_CommandCount; i++)
        {
            RenderCommandFn function = *(RenderCommandFn*)buffer;
            buffer += sizeof(RenderCommandFn);

            uint32_t size = *(uint32_t*)buffer;
            buffer += sizeof(uint32_t);
            function(buffer);
            buffer += size;
        }

        m_CommandBufferPtr = m_CommandBuffer;
        m_CommandCount = 0;
    }

    //    scope<RenderCommandQueue> RendererCommand::s_CommandQueue = nullptr;
    scope<RingBuffer> RendererCommand::s_CommandQueue;

    void RendererCommand::Init()
    {
        s_CommandQueue = new_scope<RingBuffer>();
    }

    void RendererCommand::Shutdown()
    {
    }

    void RendererCommand::WaitAndRender()
    {
//        FOX_PROFILE_FUNC();
        s_CommandQueue->Execute();
    }
}
