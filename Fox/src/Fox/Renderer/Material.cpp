//
// Created by samuel on 28/10/22.
//

#include "Material.hpp"

namespace fox
{
    Material::Material(Ref<Shader> shader) : m_Shader(shader)
    {
    }

    void Material::UpdateForRendering()
    {
        m_Shader->Bind();
        for (auto uniform : m_UniformsTextures) {
            auto texture = uniform.second.Texture;
            auto slotIndex = uniform.second.slotIndex;
            if (texture) {
                texture->Bind(slotIndex);
            }
        }
        for (auto uniform : m_Uniforms)
        {
            GetDataAsValue(uniform.second, [&uniform, this](auto value) {
                m_Shader->SetUniform(uniform.first, value);
            });
        }
    }

    void Material::EndRendering()
    {
        m_Shader->Unbind();
    }

    void Material::UpdateBuffer()
    {
        m_AttributesBuffer.reset();
        for (auto& it : m_AttributesInfo)
        {
            m_AttributesBuffer.write(it.second.data, it.second.dataSize);
        }
    }

    //void Material::SetAttribute(int index, float value)
    //{
    //    AttributeInfo* attribute;
    //    auto& it = m_AttributesInfo.find(index);
    //    if (it == m_AttributesInfo.end())
    //    {
    //        // For now add a new attribute but I may change it to return and move this code to a new function
    //        m_AttributesInfo[index] = {};
    //        attribute = &m_AttributesInfo[index];
    //    }
    //    attribute = &it->second;

    //    std::copy(value, value + sizeof(float), attribute->data);
    //    attribute->dataSize = sizeof(float);
    //}

    void Material::Set(const std::string &name, float v0)
    {
        m_Uniforms[name] = {
            UniformDataType::Float,
            Utils::ValueWrapper(v0)
        };
    }

    void Material::Set(const std::string &name, float v0, float v1)
    {
        m_Uniforms[name] = {
            UniformDataType::Float2,
            Utils::ValueWrapper(glm::vec2(v0, v1))
        };
    }

    void Material::Set(const std::string &name, float v0, float v1, float v2)
    {
        m_Uniforms[name] = {
            UniformDataType::Float3,
            Utils::ValueWrapper(glm::vec3(v0, v1, v2))
        };
    }

    void Material::Set(const std::string &name, float v0, float v1, float v2, float v3)
    {
        m_Uniforms[name] = {
            UniformDataType::Float4,
            Utils::ValueWrapper(glm::vec4(v0, v1, v2, v3))
        };
    }

    void Material::Set(const std::string &name, int v0)
    {
        m_Uniforms[name] = {
            UniformDataType::Int,
            Utils::ValueWrapper(v0)
        };
    }

    void Material::Set(const std::string &name, const glm::vec2& value)
    {
        Set(name, value.x, value.y);
    }

    void Material::Set(const std::string &name, const glm::vec3& value)
    {
        Set(name, value.x, value.y, value.z);
    }

    void Material::Set(const std::string &name, const glm::vec4& value)
    {
        Set(name, value.x, value.y, value.z, value.w);
    }

    void Material::Set(const std::string &name, const glm::mat3& value)
    {
        m_Uniforms[name] = {
            UniformDataType::Mat3,
            Utils::ValueWrapper(value)
        };
    }

    void Material::Set(const std::string &name, const glm::mat4& value)
    {
        m_Uniforms[name] = {
            UniformDataType::Mat4,
            Utils::ValueWrapper(value)
        };
    }

    void Material::Set(const std::string &name, const Ref<Texture2D>& value, uint32_t slotIndex)
    {
//        TextureMat* textureMat = new TextureMat();
//        textureMat->Texture = value;
//        textureMat->slotIndex = slotIndex;

//        m_Uniforms[name] = {
//            UniformDataType::Texture,
//            Utils::ValueWrapper((void*)textureMat, sizeof(TextureMat))
//        };

        m_UniformsTextures[name] = {
            value,
            slotIndex,
        };
    }
}
