//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_SHADER_HPP
#define FOXENGINE_SHADER_HPP

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace fox
{
    class ShaderProgramSource;
    class Shader
    {
    public:
        explicit Shader(const std::string &filepath);

        ~Shader();

        void Bind() const;

        void Unbind() const;

        void SetUniform(const std::string &name, float v0);
        void SetUniform(const std::string &name, float v0, float v1);
        void SetUniform(const std::string &name, float v0, float v1, float v2);
        void SetUniform(const std::string& name, float v0, float v1, float v2, float v3);
        void SetUniform(const std::string &name, float* v, uint32_t count);

        void SetUniform(const std::string &name, double v0);
        void SetUniform(const std::string &name, double v0, double v1);
        void SetUniform(const std::string &name, double v0, double v1, double v2);
        void SetUniform(const std::string& name, double v0, double v1, double v2, double v3);
        void SetUniform(const std::string &name, double* v, uint32_t count);

        void SetUniform(const std::string &name, int v0);
        void SetUniform(const std::string &name, int v0, int v1);
        void SetUniform(const std::string &name, int v0, int v1, int v2);
        void SetUniform(const std::string &name, int v0, int v1, int v2, int v3);
        void SetUniform(const std::string &name, int* v, uint32_t count);

        void SetUniform(const std::string &name, const glm::vec2& value);
        void SetUniform(const std::string &name, const glm::vec3& value);
        void SetUniform(const std::string &name, const glm::vec4& value);
        void SetUniform(const std::string &name, const glm::mat2& value);
        void SetUniform(const std::string &name, const glm::mat3& value);
        void SetUniform(const std::string &name, const glm::mat4& value);
//        void SetUniform(const char* name, std::span<const int> values)
//        void SetUniform(const char* name, std::span<const float> values);
//        void SetUniform(const char* name, std::span<const vec2> values);
//        void SetUniform(const char* name, std::span<const vec3> values);
//        void SetUniform(const char* name, std::span<const vec4> values);
//        void SetUniform(const char* name, std::span<const mat2> values);
//        void SetUniform(const char* name, std::span<const mat3> values);
//        void SetUniform(const char* name, std::span<const mat4> values);

    private:
        int GetUniformLocation(const std::string& name) const;
        int CreateShader(const std::string& vertexShader, const std::string& fragShader);
        unsigned int CompileShader(unsigned int type, const std::string& source);
        ShaderProgramSource ParseShader(const std::string& filepath);

        std::string m_strFilepath;
        unsigned int m_RendererID;
        mutable std::unordered_map<std::string, int> m_LocationCache;
    };
}

#endif //FOXENGINE_SHADER_HPP
