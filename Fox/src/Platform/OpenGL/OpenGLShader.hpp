//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_OPENGLSHADER_HPP
#define FOXENGINE_OPENGLSHADER_HPP

#include <string>
#include <unordered_map>
#include "Renderer/Shader.hpp"
#include <glad/glad.h>

namespace fox
{
    class ShaderProgramSource;
    class OpenGLShader : public Shader
    {
    public:
        explicit OpenGLShader(const std::string& path);
        explicit OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc);

        ~OpenGLShader() override;

        void Bind() const override;

        void Unbind() const override;

        const std::string& GetName() const override { return m_strName; }

        void SetUniform(const std::string &name, float v0) override;
        void SetUniform(const std::string &name, float v0, float v1) override;
        void SetUniform(const std::string &name, float v0, float v1, float v2) override;
        void SetUniform(const std::string& name, float v0, float v1, float v2, float v3) override;
        void SetUniform(const std::string &name, float* v, uint32_t count) override;

        void SetUniform(const std::string &name, double v0) override;
        void SetUniform(const std::string &name, double v0, double v1) override;
        void SetUniform(const std::string &name, double v0, double v1, double v2) override;
        void SetUniform(const std::string& name, double v0, double v1, double v2, double v3) override;
        void SetUniform(const std::string &name, double* v, uint32_t count) override;

        void SetUniform(const std::string &name, int v0) override;
        void SetUniform(const std::string &name, int v0, int v1) override;
        void SetUniform(const std::string &name, int v0, int v1, int v2) override;
        void SetUniform(const std::string &name, int v0, int v1, int v2, int v3) override;
        void SetUniform(const std::string &name, int* v, uint32_t count) override;

        void SetUniform(const std::string &name, const glm::vec2& value) override;
        void SetUniform(const std::string &name, const glm::vec3& value) override;
        void SetUniform(const std::string &name, const glm::vec4& value) override;
        void SetUniform(const std::string &name, const glm::mat2& value) override;
        void SetUniform(const std::string &name, const glm::mat3& value) override;
        void SetUniform(const std::string &name, const glm::mat4& value) override;

    private:
        int GetUniformLocation(const std::string& name) const;

        std::string ReadFile(const std::string& filepath);
        void CompileShader(const std::unordered_map<GLenum, std::string>& shaderSources);
        std::unordered_map<GLenum, std::string> ParseShader(const std::string& source);

        std::string m_strName;
        std::string m_strFilepath;
        unsigned int m_RendererID;
        mutable std::unordered_map<std::string, int> m_LocationCache;
    };
}

#endif //FOXENGINE_OPENGLSHADER_HPP
