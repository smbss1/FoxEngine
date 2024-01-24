//
// Created by samuel on 26/06/2021.
//

#include <fstream>
#include <glad/glad.h>
#include <Core/Logger/Logger.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Core/Assert.hpp>
#include <Utils/Path.hpp>

#include "OpenGLUtils.hpp"
#include "OpenGLShader.hpp"

namespace fox
{
    namespace Utils {
        static GLenum ShaderTypeFromString(const std::string& type)
        {
            if (type == "vertex")
                return GL_VERTEX_SHADER;
            if (type == "fragment" || type == "pixel")
                return GL_FRAGMENT_SHADER;

            FOX_CORE_ASSERT(false, "Unknown shader type!");
            return 0;
        }
    }

    OpenGLShader::OpenGLShader(const std::string &path)
        : m_RendererID(0), m_strFilepath(path), m_strName()
    {
        std::string source = ReadFile(path);
        auto shaderSources = ParseShader(source);
        CompileShader(shaderSources);

        Path oPathFile(path);
        m_strName = oPathFile.basename();
    }

    OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragSrc)
        : m_strName(name)
    {
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragSrc;
        CompileShader(sources);
    }

    OpenGLShader::~OpenGLShader()
    {
        GLCall(glUseProgram(0));
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

    void OpenGLShader::SetUniform(const std::string &name, float v0) const
    {
        GLCall(glUniform1f(GetUniformLocation(name), v0));
    }

    void OpenGLShader::SetUniform(const std::string &name, float v0, float v1) const
    {
        GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
    }

    void OpenGLShader::SetUniform(const std::string &name, float v0, float v1, float v2) const
    {
        GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
    }

    void OpenGLShader::SetUniform(const std::string &name, float v0, float v1, float v2, float v3) const
    {
        GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void OpenGLShader::SetUniform(const std::string &name, float* v, uint32_t count) const
    {
        GLCall(glUniform1fv(GetUniformLocation(name), count, v));
    }


    void OpenGLShader::SetUniform(const std::string &name, double v0) const
    {
        GLCall(glUniform1d(GetUniformLocation(name), v0));
    }

    void OpenGLShader::SetUniform(const std::string &name, double v0, double v1) const
    {
        GLCall(glUniform2d(GetUniformLocation(name), v0, v1));
    }

    void OpenGLShader::SetUniform(const std::string &name, double v0, double v1, double v2) const
    {
        GLCall(glUniform3d(GetUniformLocation(name), v0, v1, v2));
    }

    void OpenGLShader::SetUniform(const std::string &name, double v0, double v1, double v2, double v3) const
    {
        GLCall(glUniform4d(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void OpenGLShader::SetUniform(const std::string &name, double* v, uint32_t count) const
    {
        GLCall(glUniform1dv(GetUniformLocation(name), count, v));
    }


    void OpenGLShader::SetUniform(const std::string &name, int v0) const
    {
        GLCall(glUniform1i(GetUniformLocation(name), v0));
    }

    void OpenGLShader::SetUniform(const std::string &name, int v0, int v1) const
    {
        GLCall(glUniform2i(GetUniformLocation(name), v0, v1));
    }

    void OpenGLShader::SetUniform(const std::string &name, int v0, int v1, int v2) const
    {
        GLCall(glUniform3i(GetUniformLocation(name), v0, v1, v2));
    }

    void OpenGLShader::SetUniform(const std::string &name, int v0, int v1, int v2, int v3) const
    {
        GLCall(glUniform4i(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void OpenGLShader::SetUniform(const std::string &name, int* v, uint32_t count) const
    {
        GLCall(glUniform1iv(GetUniformLocation(name), count, v));
    }


    void OpenGLShader::SetUniform(const std::string &name, const glm::vec2& value) const
    {
        SetUniform(name, value.x, value.y);
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::vec3& value) const
    {
        SetUniform(name, value.x, value.y, value.z);
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::vec4& value) const
    {
        SetUniform(name, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::mat2& value) const
    {
        GLCall(glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)));
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::mat3& value) const
    {
        GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)));
    }

    void OpenGLShader::SetUniform(const std::string &name, const glm::mat4& value) const
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
            FOX_CORE_WARN("Uniform '%' doesn't exist!", name);
        m_LocationCache[name] = location;
        return location;
    }

    std::string OpenGLShader::ReadFile(const std::string& filepath)
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
            else
            {
                FOX_CORE_ERROR("Could not read from file '%'", filepath);
            }
        }
        else
        {
            FOX_CORE_ERROR("Could not open file '%'", filepath);
        }

        return result;
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::ParseShader(const std::string& source)
    {
        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
            FOX_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
            std::string type = source.substr(begin, eol - begin);
            FOX_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
            FOX_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

            shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    void OpenGLShader::CompileShader(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        GLCall(GLuint program = glCreateProgram());

        std::vector<GLuint> glShaderIDs;
        glShaderIDs.reserve(shaderSources.size());

        for (auto& kv : shaderSources)
        {
            GLenum type = kv.first;
            const std::string& source = kv.second;

            GLCall(GLuint shader = glCreateShader(type));

            const GLchar* sourceCstr = source.c_str();
            GLCall(glShaderSource(shader, 1, &sourceCstr, nullptr));
            GLCall(glCompileShader(shader));

            GLint isCompiled = 0;
            GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));
            if (isCompiled == GL_FALSE)
            {
                int length = 0;
                GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));

                std::vector<GLchar> message(length);

                GLCall(glGetShaderInfoLog(shader, length, &length, message.data()));
                FOX_CORE_ERROR("Failed to compile shader: %", message.data());
                FOX_CORE_ASSERT(false, "Shader compilation failure!");
                GLCall(glDeleteShader(shader));
                return;
            }

            glAttachShader(program, shader);
            glShaderIDs.push_back(shader);
        }

        //GLCall(glBindAttribLocation(program, 0, "a_Position"));

        // example of struct for size of buffer data
        // it's not a struct who will be in code
        // but this struct must be the representation of what m_attributeData contains
        //struct TestInstance
        //{
        //    glm::vec4 color;
        //    float texIndex;
        //};

        //struct Data
        //{
        //    void* data;
        //    uint8_t size;
        //};

        //std::vector<Data> m_attributeData;
        //auto g = glm::vec4(1, 0, 1, 1);
        //m_attributeData.push_back({ glm::value_ptr(g), sizeof(glm::vec4) }); // color attribute
        //m_attributeData.push_back({ (void*)5, sizeof(float)}); // TexIndex attribute

        // so here m_attributeData.size() should be equal to sizeof(TestInstance)

        // Later when sending the data to the vbo who hold the dynami per-instance data

        // Push the mesh and the m_attributeData

        // Link our program
        GLCall(glLinkProgram(program));

        GLint isLinked = 0;
        GLCall(glGetProgramiv(program, GL_LINK_STATUS, &isLinked));
        if (isLinked == GL_FALSE)
        {
            int length = 0;
            GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));

            std::vector<GLchar> message(length);

            GLCall(glGetProgramInfoLog(program, length, &length, message.data()));

            GLCall(glDeleteProgram(program));

            for (auto id : glShaderIDs) {
                GLCall(glDeleteShader(id));
            }

            FOX_CORE_ERROR("Failed to link program: %", message.data());
            FOX_CORE_ASSERT(false, "Shader link failure!");
            return;
        }

        GLCall(glValidateProgram(program));

        //// Get Attribute Locations
        //GLint numAttribs;
        //glGetProgramInterfaceiv(program, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

        //GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

        //printf("Active attributes:\n");
        //for (int i = 0; i < numAttribs; ++i) {
        //    GLint results[3];
        //    glGetProgramResourceiv(program, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

        //    GLint nameBufSize = results[0] + 1;
        //    char* name = new char[nameBufSize];
        //    glGetProgramResourceName(program, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
        //    printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
        //    delete[] name;
        //}



        for (auto idd : glShaderIDs) {
            GLCall(glDetachShader(program, idd));
            GLCall(glDeleteShader(idd));
        }

        m_RendererID = program;
    }

    const char* OpenGLShader::getTypeString(GLenum type)
    {
        // There are many more types than are covered here, but
        // these are the most common in these examples.
        switch (type) {
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_DOUBLE:
            return "double";
        case GL_INT:
            return "int";
        case GL_UNSIGNED_INT:
            return "unsigned int";
        case GL_BOOL:
            return "bool";
        case GL_FLOAT_MAT2:
            return "mat2";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";
        default:
            return "?";
        }
    }
}
