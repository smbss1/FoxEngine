//
// Created by samuel on 25/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "Renderer/Texture.hpp"
#include "OpenGL/OpenGLTexture.hpp"

namespace fox
{
	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer data)
	{
        return new_ref<OpenGLTexture>(specification, data);
	}
}
