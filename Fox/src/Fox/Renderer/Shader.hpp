//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_SHADER_HPP
#define FOXENGINE_SHADER_HPP

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

namespace fox
{
    class Shader : public RefCounted
    {
    public:
        virtual ~Shader() = default;

        virtual const uint32_t& GetID() const = 0;
        virtual const std::string& GetName() const = 0;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetUniform(const std::string &name, float v0) const = 0;
        virtual void SetUniform(const std::string &name, float v0, float v1) const = 0;
        virtual void SetUniform(const std::string &name, float v0, float v1, float v2) const = 0;
        virtual void SetUniform(const std::string& name, float v0, float v1, float v2, float v3) const = 0;
        virtual void SetUniform(const std::string &name, float* v, uint32_t count) const = 0;

        virtual void SetUniform(const std::string &name, double v0) const = 0;
        virtual void SetUniform(const std::string &name, double v0, double v1) const = 0;
        virtual void SetUniform(const std::string &name, double v0, double v1, double v2) const = 0;
        virtual void SetUniform(const std::string& name, double v0, double v1, double v2, double v3) const = 0;
        virtual void SetUniform(const std::string &name, double* v, uint32_t count) const = 0;

        virtual void SetUniform(const std::string &name, int v0) const = 0;
        virtual void SetUniform(const std::string &name, int v0, int v1) const = 0;
        virtual void SetUniform(const std::string &name, int v0, int v1, int v2) const = 0;
        virtual void SetUniform(const std::string &name, int v0, int v1, int v2, int v3) const = 0;
        virtual void SetUniform(const std::string &name, int* v, uint32_t count) const = 0;

        virtual void SetUniform(const std::string &name, const glm::vec2& value) const = 0;
        virtual void SetUniform(const std::string &name, const glm::vec3& value) const = 0;
        virtual void SetUniform(const std::string &name, const glm::vec4& value) const = 0;
        virtual void SetUniform(const std::string &name, const glm::mat2& value) const = 0;
        virtual void SetUniform(const std::string &name, const glm::mat3& value) const = 0;
        virtual void SetUniform(const std::string &name, const glm::mat4& value) const = 0;

        static Ref<Shader> Create(const std::string& path);
        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc);
    };

    class ShaderLibrary
    {
    public:
        void Add(const Ref<Shader>& pShader);
        void Add(const std::string& name, const Ref<Shader>& pShader);
        Ref<Shader> Load(const std::string& filepath);
        Ref<Shader> Load(const std::string& name, const std::string& filepath);

        Ref<Shader> Get(const std::string& name);
        bool Exists(const std::string& name);
    private:
        std::unordered_map<std::string, Ref<Shader>> m_vShaders;
    };
}

#endif //FOXENGINE_SHADER_HPP
