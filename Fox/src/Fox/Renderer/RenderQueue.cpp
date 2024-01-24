#include "RenderQueue.hpp"
#include "Renderer/RenderQueue.hpp"
#include "common.hpp"

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

    void RenderCommandQueue::Clear()
    {
        m_CommandBufferPtr = m_CommandBuffer;
        m_CommandCount = 0;
    }

    void RenderCommandQueue::Destroy()
    {
        delete[] m_CommandBuffer;
    }

    void RenderCommandQueue::Execute()
    {
        byte* buffer = m_CommandBuffer;

        for (uint32_t i = 0; i < m_CommandCount; i++)
        {
            // Interpret the current part of the buffer as a CommandPacket
            CommandPacket* packet = reinterpret_cast<CommandPacket*>(buffer);

            // Move the buffer pointer ahead by the size of a CommandPacket
            buffer += sizeof(CommandPacket);

            // Ensure alignment for the command data
            buffer = reinterpret_cast<byte*>((reinterpret_cast<uintptr_t>(buffer) + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1));

            // Call the dispatch function on the command data
            packet->dispatchFunction(packet->commandData);

            // Move the buffer pointer ahead by the size of the command data
            buffer += packet->commandSize; // Assuming all commands are of uniform size; adjust if not
        }

        // Reset for the next frame
        m_CommandBufferPtr = m_CommandBuffer;
        m_CommandCount = 0;
    }
}