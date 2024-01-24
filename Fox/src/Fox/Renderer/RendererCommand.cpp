//
// Created by samuel on 27/06/2021.
//

#include "RendererCommand.hpp"

namespace fox
{
    scope<RingBuffer> RendererCommand::s_CommandQueue;

    void RendererCommand::Init()
    {
        s_CommandQueue = new_scope<RingBuffer>();
    }

    void RendererCommand::Shutdown()
    {
        s_CommandQueue = nullptr;
    }

    void RendererCommand::WaitAndRender()
    {
//        FOX_PROFILE_FUNC();
        s_CommandQueue->Execute();
    }
}
