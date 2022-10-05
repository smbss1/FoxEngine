

#ifndef FOXEGINE_OPENGL_UTILS
#define FOXEGINE_OPENGL_UTILS

#include "Renderer/RendererAPI.hpp"
#include "DebugBreak.hpp"

#include <glad/glad.h>
#include <string>

#define ASSERT(x) if (!(x)) debug_break();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

#endif
