//
// Created by samuel on 27/06/2021.
//

#include "Renderer/RendererAPI.hpp"
#include "OpenGL/OpenGLRendererAPI.hpp"

namespace fox
{
    scope<RendererAPI> RendererAPI::Create()
	{
        return new_scope<OpenGLRendererAPI>();

		// switch (s_API)
		// {
		// 	case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		// 	case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		// }

		// HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		// return nullptr;
	}
}
