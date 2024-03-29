//
// Created by samuel on 27/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include <Core/Assert.hpp>
#include "Renderer/Shader.hpp"
#include "OpenGL/OpenGLShader.hpp"

namespace fox
{
    Ref<Shader> Shader::Create(const std::string &path)
    {
        return new_ref<OpenGLShader>(path);
    }

    Ref<Shader> Shader::Create(const std::string &name, const std::string &vertexSrc, const std::string &fragSrc)
    {
        return new_ref<OpenGLShader>(name, vertexSrc, fragSrc);
    }

    void ShaderLibrary::Add(const Ref<Shader> &pShader)
    {
        auto& name = pShader->GetName();
        Add(name, pShader);
    }

    void ShaderLibrary::Add(const std::string &name, const Ref<Shader> &pShader)
    {
        FOX_CORE_ASSERT(!Exists(name), "Shader already exists!");
        m_vShaders[name] = pShader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string &filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string &name, const std::string &filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string &name)
    {
        FOX_CORE_ASSERT(Exists(name), "Shader not found!");
        return m_vShaders[name];
    }

    bool ShaderLibrary::Exists(const std::string &name)
    {
        return m_vShaders.find(name) != m_vShaders.end();
    }
}
