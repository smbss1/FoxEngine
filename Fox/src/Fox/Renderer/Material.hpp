//
// Created by samuel on 28/10/22.
//

#ifndef FOX_LEXER_MATERIAL_HPP
#define FOX_LEXER_MATERIAL_HPP

#include "Scripting/ValueWrapper.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <unordered_map>
#include <string>

namespace fox
{
    enum class UniformDataType
    {
        None,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool,
        Texture
    };

    struct TextureMat
    {
        Ref<Texture2D> Texture;
        uint32_t slotIndex;
    };


    struct ShaderData
    {
        ShaderData() = default;

        UniformDataType Type;
        Utils::ValueWrapper Value;
    };

    template <typename Fn>
    inline void GetDataAsValue(const ShaderData& data, Fn&& function)
    {
        switch (data.Type)
        {
            case UniformDataType::Int:
                function(data.Value.Get<int>());
                return;
            case UniformDataType::Bool:
                function(data.Value.Get<bool>());
                return;
            case UniformDataType::Float:
                function(data.Value.Get<float>());
                return;
            case UniformDataType::Float2:
                function(data.Value.Get<glm::vec2>());
                return;
            case UniformDataType::Float3:
                function(data.Value.Get<glm::vec3>());
                return;
            case UniformDataType::Float4:
                function(data.Value.Get<glm::vec4>());
                return;
            case UniformDataType::Mat3:
                function(data.Value.Get<glm::mat3>());
                return;
            case UniformDataType::Mat4:
                function(data.Value.Get<glm::mat4>());
                return;
//            case UniformDataType::Texture:
//                function(data.Value.Get<glm::mat4>());
//                return;
            default:
                FOX_ERROR("ShaderDataType (%) not supported.", (int)data.Type);
                break;
        }
    }

    class Material : public RefCounted
    {
    public:
        explicit Material(Ref<Shader> shader);
        ~Material() = default;

        void Set(const std::string &name, float v0);
        void Set(const std::string &name, float v0, float v1);
        void Set(const std::string &name, float v0, float v1, float v2);
        void Set(const std::string& name, float v0, float v1, float v2, float v3);

        void Set(const std::string &name, int v0);
        void Set(const std::string &name, const glm::vec2& value);
        void Set(const std::string &name, const glm::vec3& value);
        void Set(const std::string &name, const glm::vec4& value);
        void Set(const std::string &name, const glm::mat3& value);
        void Set(const std::string &name, const glm::mat4& value);
        void Set(const std::string &name, const Ref<Texture2D>& value, uint32_t slotIndex = 0);

        void UpdateForRendering();
    private:
        Ref<Shader> m_Shader;
        std::unordered_map<std::string, ShaderData> m_Uniforms;
        std::unordered_map<std::string, TextureMat> m_UniformsTextures;
    };
}

#endif //FOX_LEXER_MATERIAL_HPP
