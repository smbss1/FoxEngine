//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_RENDERERCOMMAND_HPP
#define FOXENGINE_RENDERERCOMMAND_HPP

#include <glm/vec4.hpp>
#include "common.hpp"

namespace fox
{
    class RenderCommandQueue
    {
    public:
        typedef void(*RenderCommandFn)(void*);

        RenderCommandQueue();
        ~RenderCommandQueue();

        void* Push(RenderCommandFn func, uint32_t size);

        void Execute();
    private:
        uint8_t* m_CommandBuffer;
        uint8_t* m_CommandBufferPtr;
        uint32_t m_CommandCount = 0;
    };

    class RendererCommand
    {
    public:
        static void Init();
        static void Shutdown();
        static void WaitAndRender();

        template<typename FuncT>
        static void Submit(FuncT&& func)
        {
            auto renderCmd = [](void* ptr) {
                auto pFunc = (FuncT*)ptr;
                (*pFunc)();

                // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
                // however some items like uniforms which contain std::strings still exist for now
                // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
                pFunc->~FuncT();
            };
            auto storageBuffer = s_CommandQueue->Push(renderCmd, sizeof(func));
            new (storageBuffer) FuncT(std::forward<FuncT>(func));
        }

    private:
        static scope<RenderCommandQueue> s_CommandQueue;
    };
}

#endif //FOXENGINE_RENDERERCOMMAND_HPP
