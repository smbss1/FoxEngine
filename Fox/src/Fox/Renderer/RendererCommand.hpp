//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_RENDERERCOMMAND_HPP
#define FOXENGINE_RENDERERCOMMAND_HPP

#include <glm/vec4.hpp>
#include "common.hpp"
#include "Utils/RingBuffer.hpp"

namespace fox
{
    class RendererCommand
    {
    public:
        static void Init();
        static void Shutdown();
        static void WaitAndRender();

//        template<typename FuncT>
//        static void Submit(FuncT&& func)
//        {
//            auto renderCmd = [](void* ptr) {
//                auto pFunc = (FuncT*)ptr;
//                (*pFunc)();
//
//                // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
//                // however some items like uniforms which contain std::strings still exist for now
//                // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
//                pFunc->~FuncT();
//            };
//            auto storageBuffer = s_CommandQueue->Push(renderCmd, sizeof(func));
//            new (storageBuffer) FuncT(std::forward<FuncT>(func));
//        }

        template<typename T, typename... Args>
        static void Push(Args&&... args)
        {
            s_CommandQueue->Push<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        static void Push(T&& cmd)
        {
            s_CommandQueue->Push<T>(std::move(cmd));
        }

    private:
        static scope<RingBuffer> s_CommandQueue;
    };
}

#endif //FOXENGINE_RENDERERCOMMAND_HPP
