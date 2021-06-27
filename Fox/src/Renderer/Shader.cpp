//
// Created by samuel on 27/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "Renderer/Shader.hpp"

namespace fox
{
    ref<Shader> Shader::Create(const std::string& path)
    {
        return RendererAPI::GetGraphicPlugin()->CreateShader(path);
    }
}