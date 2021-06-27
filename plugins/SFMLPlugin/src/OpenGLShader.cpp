//
// Created by samuel on 26/06/2021.
//

#include <fstream>
#include <glad/glad.h>
#include <Core/Logger/Logger.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <OpenGLRendererAPI.hpp>
#include "OpenGLShader.hpp"

namespace fox
{
    struct ShaderProgramSource
    {
        std::string VertexSource;
        std::string FragmentSource;
    };

    OpenGLShader::OpenGLShader(const std::string &filepath)
        : m_RendererID(0), m_strFilepath(filepath)
    {
        ShaderProgramSource source = ParseShader(m_strFilepath);
        m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
    }

    OpenGLShader::~OpenGLShader()
    {
        GLCall(glDeleteProgram(m_RendererID));
    }

    void OpenGLShader::Bind() const
    {
        GLCall(glUseProgram(m_RendererID));
    }

    void OpenGLShader::Unbind() const
    {
        GLCall(glUseProgram(0));
    }

    void OpenGLShader::SetUniform(const std::string &name, float v0)
    {
        GLCall(glUniform1f(GetUniformLocation(name), v0));
    }

    void OpenGLShader::SetUniform(const std::string &name, float v0, float v1)
    {
        GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
    }

    void OpenGLShader::SetUniform(const std::string &name, float v0, float v1, float v2)
    {
        GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
    }

    void OpenGLShader::SetUniform(const std::string &name, float v0, float v1, float v2, float v3)
    {
        GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void OpenGLShader::SetUniform(const std::string &name, float* v, uint32_t count)
    {
        GLCall(glUniform1fv(GetUniformLocation(name), count, v));
    }


    void OpenGLShader::SetUniform(const std::string &name, double v0)
    {
        GLCall(glUniform1d(GetUniformLocation(name), v0));
    }

    void OpenGLShader::SetUniform(const std::string &name, double v0, double v1)
    {
        GLCall(glUniform2d(GetUniformLocation(name), v0, v1));
    }

    void OpenGLShader::SetUniform(const std::string &name, double v0, double v1, double v2)
    {
        GLCall(glUniform3d(GetUniformLocation(name), v0, v1, v2));
    }

    void OpenGLShader::SetUniform(const std::string &name, double v0, double v1, double v2, double v3)
    {
        GLCall(glUniform4d(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void OpenGLShader::SetUniform(const std::string &name, double* v, uint32_t count)
    {
        GLCall(glUniform1dv(GetUniformLocation(name), count, v));
    }


    void OpenGLShader::SetUniform(const std::string &name, int v0)
    {
        GLCall(glUniform1i(GetUniformLocation(name), v0));
    }

    void OpenGLShader::SetUniform(const std::string &name, int v0, int v1)
    {
        GLCall(glUniform2i(GetUniformLocation(name), v0, v1));
    }

    void OpenGLShader::SetUniform(const std::string &name, int v0, int v1, int v2)
    {
        GLCall(glUniform3i(GetUniformLocation(name), v0, v1, v2));
    }

    void OpenGLShader::SetUniform(const std::string &name, int v0, int v1, int v2, int v3)
    {
        GLCall(glUniform4i(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void OpenGLShader::SetUniform(const std::string &name, int* v, uint32_t count)
    {
        GLCall(glUniform1iv(GetUniformLocation(name), count, v));
    }


    void OpenGLShader::SetUniform(const std::string &name, const glm::vec2& value)
    {
        SetUniform(name, value.x, value.y);
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::vec3& value)
    {
        SetUniform(name, value.x, value.y, value.z);
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::vec4& value)
    {
        SetUniform(name, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::mat2& value)
    {
        GLCall(glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)));
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::mat3& value)
    {
        GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)));
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::mat4& value)
    {
        GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)));
    }


    int OpenGLShader::GetUniformLocation(const std::string &name) const
    {
        if (m_LocationCache.find(name) != m_LocationCache.end())
            return m_LocationCache[name];
        int location;
        GLCall(location = glGetUniformLocation(m_RendererID, name.c_str()));
        if (location == -1)
            fox::warn("Uniform '%' doesn't exist!", name);
        m_LocationCache[name] = location;
        return location;
    }

    ShaderProgramSource OpenGLShader::ParseShader(const std::string& filepath)
    {
        std::ifstream stream(filepath);

        enum class ShaderType
        {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        while (getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                    type = ShaderType::VERTEX;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::FRAGMENT;
            }
            else
            {
                if (type != ShaderType::NONE)
                    ss[(int)type] << line << '\n';
            }
        }
        return {ss[0].str(), ss[1].str()};
    }

    unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source)
    {
        // Gen a id as a type of shader
        GLCall(unsigned int id = glCreateShader(type));
        const char* src = source.c_str();
        // Set the shader source string to the shader
        GLCall(glShaderSource(id, 1, &src, nullptr));
        GLCall(glCompileShader(id));

        int result;
        GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE)
        {
            int length;
            GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            std::string message(' ', length);
            GLCall(glGetShaderInfoLog(id, length, &length, message.data()));
            fox::error("Failed to compile % shader: %",
                       (type == GL_VERTEX_SHADER ? "vertex":"fragment"), message);
            GLCall(glDeleteShader(id));
            return 0;
        }

        return id;
    }

    int OpenGLShader::CreateShader(const std::string& vertexShader, const std::string& fragShader)
    {
        GLCall(unsigned int program = glCreateProgram());
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

        GLCall(glAttachShader(program, vs));
        GLCall(glAttachShader(program, fs));
        GLCall(glLinkProgram(program));
        GLCall(glValidateProgram(program));

        GLCall(glDeleteShader(vs));
        GLCall(glDeleteShader(fs));

        return program;
    }
}