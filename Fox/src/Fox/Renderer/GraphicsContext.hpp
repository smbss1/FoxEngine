//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_GRAPHICSCONTEXT_HPP
#define FOXENGINE_GRAPHICSCONTEXT_HPP

#include "common.hpp"

namespace fox
{
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static scope<GraphicsContext> Create(void* window);
    };
}

#endif //FOXENGINE_GRAPHICSCONTEXT_HPP
