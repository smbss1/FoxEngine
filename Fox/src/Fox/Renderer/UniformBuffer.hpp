//
// Created by samuel on 03/10/22.
//

#ifndef FOXENGINE_UNIFORMBUFFER_HPP
#define FOXENGINE_UNIFORMBUFFER_HPP

#include <cstdint>
#include "common.hpp"

namespace fox
{
    class UniformBuffer : public RefCounted
    {
    public:
        virtual ~UniformBuffer() {}
        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

        static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
    };
}


#endif //FOX_LEXER_UNIFORMBUFFER_HPP
