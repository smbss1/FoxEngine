//
// Created by samuel on 28/06/2021.
//

#include "common.hpp"
#include <Renderer/Renderer2D.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Buffer.hpp>
#include <Renderer/RendererCommand.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fox
{
    struct QuadVertex
    {
        glm::vec3 oPosition;
        glm::vec4 oColor;
        glm::vec2 oTexCoord;
        float fTexIndex;
        float fTilingFactor;
    };

    struct Renderer2DData
    {
        static const uint32_t  MaxQuads = 10000;
        static const uint32_t  MaxVertices = MaxQuads * 4;
        static const uint32_t  MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

        ref<VertexArray> pQuadVertexArray;
        ref<VertexBuffer> pQuadVertexBuffer;
        ref<Shader> pTextureShader;
        ref<Texture2D> pWhiteTexture;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        std::array<ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        s_Data.pQuadVertexArray = fox::VertexArray::Create();

        s_Data.pQuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

        s_Data.pQuadVertexBuffer->SetLayout({
              {fox::ShaderDataType::Float3, "a_Position"},
              {fox::ShaderDataType::Float4, "a_Color"},
              {fox::ShaderDataType::Float2, "a_TexCoord"},
              {fox::ShaderDataType::Float, "a_TexIndex"},
              {fox::ShaderDataType::Float, "a_TilingFactor"},
          });
        s_Data.pQuadVertexArray->AddVertexBuffer(s_Data.pQuadVertexBuffer);

        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

        uint32_t offset = 0;
        for (int i = 0; i < s_Data.MaxIndices; i += 6) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
            offset += 4;
        }
        auto quadIB = fox::IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        s_Data.pQuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        s_Data.pWhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.pWhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        int32_t samplers[s_Data.MaxTextureSlots];
        for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i)
            samplers[i] = i;

        s_Data.pTextureShader = fox::Shader::Create("assets/shaders/Texture.glsl");
        s_Data.pTextureShader->Bind();
        s_Data.pTextureShader->SetUniform("u_Textures", samplers, s_Data.MaxTextureSlots);

        // Set first texture slot to 0
        s_Data.TextureSlots[0] = s_Data.pWhiteTexture;
    }

    void Renderer2D::Shutdown()
    {
        for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i)
            s_Data.TextureSlots[i].reset();
        s_Data.pQuadVertexBuffer.reset();
        s_Data.pTextureShader.reset();
        s_Data.pQuadVertexArray.reset();
        s_Data.pWhiteTexture.reset();
        delete[] s_Data.QuadVertexBufferBase;
    }

    void Renderer2D::BeginScene(const OrthographicCamera &camera)
    {
        s_Data.pTextureShader->Bind();
        s_Data.pTextureShader->SetUniform("u_ViewProjection", camera.GetViewProjectionMatrix());

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::EndScene()
    {
        uint32_t dataSize = (uint8_t *)s_Data.QuadVertexBufferPtr - (uint8_t *)s_Data.QuadVertexBufferBase;
        s_Data.pQuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
        Flush();
    }

    void Renderer2D::Flush()
    {
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
            s_Data.TextureSlots[i]->Bind(i);
        RendererCommand::DrawIndexed(s_Data.pQuadVertexArray, s_Data.QuadIndexCount);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad({position.x, position.y, 0}, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        const float textureIndex = 0.0f; // White texture
        const float tilingFactor = 1.0f; // White texture

        s_Data.QuadVertexBufferPtr->oPosition = position;
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 0.0f, 0.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->oPosition = { position.x + size.x, position.y, 0.0f };
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 1.0f, 0.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->oPosition = { position.x + size.x, position.y + size.y, 0.0f };
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 1.0f, 1.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->oPosition = { position.x, position.y + size.y, 0.0f };
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 0.0f, 1.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadIndexCount += 6;

//        s_Data.pTextureShader->SetUniform("u_Color", color);
//        s_Data.pTextureShader->SetUniform("u_TilingFactor", 1.0f);
//
//        s_Data.pWhiteTexture->Bind();
//
//        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
//                              glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});
//        s_Data.pTextureShader->SetUniform("u_Transform", transform);
//
//        s_Data.pQuadVertexArray->Bind();
//        RendererCommand::DrawIndexed(s_Data.pQuadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const ref<Texture2D> &texture,
                              const glm::vec4 &tintColor, float tilingFactor)
    {
        DrawQuad({position.x, position.y, 0}, size, texture, tintColor, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const ref<Texture2D> &texture,
                              const glm::vec4 &tintColor, float tilingFactor)
    {
        constexpr glm::vec4 color = glm::vec4(1.0f);

        float textureIndex = 0.0f;
        for (int i = 0; i < s_Data.TextureSlotIndex; ++i)
        {
            if (*s_Data.TextureSlots[i] == *texture)
            {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
        }

        s_Data.QuadVertexBufferPtr->oPosition = position;
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 0.0f, 0.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->oPosition = { position.x + size.x, position.y, 0.0f };
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 1.0f, 0.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->oPosition = { position.x + size.x, position.y + size.y, 0.0f };
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 1.0f, 1.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->oPosition = { position.x, position.y + size.y, 0.0f };
        s_Data.QuadVertexBufferPtr->oColor = color;
        s_Data.QuadVertexBufferPtr->oTexCoord = { 0.0f, 1.0f };
        s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadIndexCount += 6;
#if 0
        s_Data.pTextureShader->SetUniform("u_Color", tintColor);
        s_Data.pTextureShader->SetUniform("u_TilingFactor", tilingFactor);

        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});
        s_Data.pTextureShader->SetUniform("u_Transform", transform);

        s_Data.pQuadVertexArray->Bind();
        RendererCommand::DrawIndexed(s_Data.pQuadVertexArray);
#endif
    }

    void Renderer2D::ResetStats()
    {
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return Renderer2D::Statistics();
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                     const glm::vec4 &color)
    {
        DrawRotatedQuad({position.x, position.y, 0}, size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                     const glm::vec4 &color)
    {
        s_Data.pTextureShader->SetUniform("u_Color", color);
        s_Data.pTextureShader->SetUniform("u_TilingFactor", 1.0f);

        s_Data.pWhiteTexture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});
        s_Data.pTextureShader->SetUniform("u_Transform", transform);

        s_Data.pQuadVertexArray->Bind();
        RendererCommand::DrawIndexed(s_Data.pQuadVertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                     const ref<Texture2D> &texture, const glm::vec4 &tintColor, float tilingFactor)
    {
        DrawRotatedQuad({position.x, position.y, 0}, size, rotation, texture, tintColor, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                     const ref<Texture2D> &texture, const glm::vec4 &tintColor, float tilingFactor)
    {
        s_Data.pTextureShader->SetUniform("u_Color", tintColor);
        s_Data.pTextureShader->SetUniform("u_TilingFactor", tilingFactor);

        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});
        s_Data.pTextureShader->SetUniform("u_Transform", transform);

        s_Data.pQuadVertexArray->Bind();
        RendererCommand::DrawIndexed(s_Data.pQuadVertexArray);
    }
}