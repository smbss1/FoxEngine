//
// Created by samuel on 06/11/22.
//

#ifndef FOX_LEXER_RINGBUFFER_HPP
#define FOX_LEXER_RINGBUFFER_HPP

#include <iostream>
#include "common.hpp"

namespace fox
{
//    struct ThreadCmd
//    {
//        virtual void Execute() = 0;
//    };

    struct ThreadCmdTest
    {
        std::string msg;

        ThreadCmdTest(std::string m) : msg(m) {}
        void Execute()
        {
            std::cout << msg << std::endl;
        }
    };

    class RingBuffer
    {
    public:
        typedef void(*RenderCommandFn)(void*);

        RingBuffer();
        ~RingBuffer();

        template<typename T, typename... Args>
        void Push(Args&&... args)
        {
            auto renderCmd = [](void* ptr) {
                auto pFunc = (T*)ptr;
                pFunc->Execute();

                // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
                // however some items like uniforms which contain std::strings still exist for now
                // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
                pFunc->~T();
            };
            auto storageBuffer = Push(renderCmd, sizeof(T));
            new (storageBuffer) T(std::forward<Args>(args)...);
        }

        template<typename T>
        void Push(T&& cmd)
        {
            auto renderCmd = [](void* ptr) {
                auto pFunc = (T*)ptr;
                pFunc->Execute();

                // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
                // however some items like uniforms which contain std::strings still exist for now
                // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
                pFunc->~T();
            };
            auto storageBuffer = Push(renderCmd, sizeof(T));
            new (storageBuffer) T(std::move(cmd));
        }

        void* Push(RenderCommandFn func, uint32_t size);

        void Execute();

    private:
        uint8_t* m_CommandBuffer;
        uint8_t* m_CommandBufferPtr;
        uint32_t m_CommandCount = 0;
    };
}


#endif //FOX_LEXER_RINGBUFFER_HPP
