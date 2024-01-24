//
// Created by samuel on 28/10/22.
//

#ifndef FOX_LEXER_MATERIAL_HPP
#define FOX_LEXER_MATERIAL_HPP

#include "Scripting/ValueWrapper.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Utils/MemoryBuffer.hpp"

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

    struct AttributeInfo
    {
        // type
        // location
        // ...

        uint8_t data[1024];
        uint32_t dataSize;
    };

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

        //void SetAttribute(int index, float value);

        template<typename T>
        void SetAttribute(int index, T* value, size_t size)
        {
            AttributeInfo* attribute;
            auto it = m_AttributesInfo.find(index);
            if (it == m_AttributesInfo.end())
            {
                // For now add a new attribute but I may change it to return and move this code to a new function
                m_AttributesInfo[index] = AttributeInfo();
                attribute = &m_AttributesInfo[index];
            }
            else
            {
                attribute = &it->second;
            }

            //std::copy(value, value + size, attribute->data);
            std::memset(attribute->data, 0, 1024);
            std::memcpy(attribute->data, value, size);
            attribute->dataSize = size;
        }

        template<typename T>
        void SetAttribute(int index, T* value)
        {
            SetAttribute(index, value, sizeof(T));
        }

        template<typename T>
        void SetAttribute(int index, const T& value)
        {
            SetAttribute(index, &value, sizeof(T));
        }

        void UpdateForRendering();
        void EndRendering();

        void UpdateBuffer();

        const Ref<Shader>& GetShader() const { return m_Shader; }
        const uint32_t& GetID() const { return m_Shader->GetID(); }
        const MemoryBuffer& GetAttributesData() const { return m_AttributesBuffer; }
        //MemoryBuffer& GetAttributesData() { return m_AttributesBuffer; }
        void SetMainTexture(const Ref<Texture>& texture) { m_MainTexture = texture; }
        const Ref<Texture>& GetMainTexture() const { return m_MainTexture; }
    private:
        Ref<Shader> m_Shader;
        std::unordered_map<std::string, ShaderData> m_Uniforms;
        std::unordered_map<std::string, TextureMat> m_UniformsTextures;
        std::map<int, AttributeInfo> m_AttributesInfo;

        Ref<Texture> m_MainTexture;
        
        MemoryBuffer m_AttributesBuffer;
    };
}

#endif //FOX_LEXER_MATERIAL_HPP
