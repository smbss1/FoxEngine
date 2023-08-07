//
// Created by samuel on 28/06/2021.
//

#include "common.hpp"
#include "UniformBuffer.hpp"
#include "Asset/AssetManager.hpp"
#include "Renderer.hpp"
#include "Material.hpp"
#include <Renderer/EditorCamera.hpp>
#include "../../../../FoxEditor/src/EditorLayer.hpp"
#include "Commands.hpp"
#include "Utils/Iterator.hpp"
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

        // Editor only
        int EntityID;
    };

    struct CircleVertex
    {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        float Thickness;
        float Fade;

        // Editor only
        int EntityID;
    };

    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;

        // Editor-only
        int EntityID;
    };

    struct Renderer2DData
    {
        static const uint32_t  MaxQuads = 20000;
        static const uint32_t  MaxVertices = MaxQuads * 4;
        static const uint32_t  MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

        Ref<VertexArray> pQuadVertexArray;
        Ref<VertexBuffer> pQuadVertexBuffer;
        Ref<Material> QuadMaterial;
        Ref<Texture2D> pWhiteTexture;

        Ref<VertexArray> CircleVertexArray;
        Ref<VertexBuffer> CircleVertexBuffer;
        Ref<Material> CircleMaterial;

        Ref<VertexArray> LineVertexArray;
        Ref<VertexBuffer> LineVertexBuffer;
        Ref<Shader> LineShader;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        uint32_t CircleIndexCount = 0;
        CircleVertex* CircleVertexBufferBase = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;

        uint32_t LineVertexCount = 0;
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;
        float LineWidth = 2.0f;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        glm::vec4 QuadVertexPositions[4];

        Renderer2D::Statistics Stats;

        Renderer2D::CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;

        Ref<Shader> PointLightShader;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        s_Data.pQuadVertexArray = VertexArray::Create();
        s_Data.pQuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
        s_Data.pQuadVertexBuffer->SetLayout({
              {fox::ShaderDataType::Float3, "a_Position"},
              {fox::ShaderDataType::Float4, "a_Color"},
              {fox::ShaderDataType::Float2, "a_TexCoord"},
              {fox::ShaderDataType::Float, "a_TexIndex"},
              {fox::ShaderDataType::Float, "a_TilingFactor"},
              {fox::ShaderDataType::Int, "a_EntityID"},
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
        auto quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        s_Data.pQuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        // Circles
        s_Data.CircleVertexArray = VertexArray::Create();
        s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
        s_Data.CircleVertexBuffer->SetLayout({
             {fox::ShaderDataType::Float3, "a_WorldPosition"},
             {fox::ShaderDataType::Float3, "a_LocalPosition"},
            {fox::ShaderDataType::Float4, "a_Color"},
            {fox::ShaderDataType::Float, "a_Thickness"},
            {fox::ShaderDataType::Float, "a_Fade"},
            {fox::ShaderDataType::Int, "a_EntityID"},
        });
        s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
        s_Data.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
        s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

        // Lines
        s_Data.LineVertexArray = VertexArray::Create();
        s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
        s_Data.LineVertexBuffer->SetLayout({
           { ShaderDataType::Float3, "a_Position" },
           { ShaderDataType::Float4, "a_Color"    },
           { ShaderDataType::Int,    "a_EntityID" }
       });
        s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];


        s_Data.pWhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.pWhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        s_Data.QuadMaterial = new_ref<Material>(Shader::Create("assets/shaders/Renderer2D_Quad.glsl"));
        s_Data.CircleMaterial = new_ref<Material>(Shader::Create("assets/shaders/Renderer2D_Circle.glsl"));
        s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");
        s_Data.PointLightShader = Shader::Create("assets/shaders/light.glsl");

        // Set first texture slot to 0
        s_Data.TextureSlots[0] = s_Data.pWhiteTexture;

        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2D::CameraData), 0);
    }

    void Renderer2D::Shutdown()
    {
        for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; ++i)
            s_Data.TextureSlots[i].Reset();
        s_Data.pQuadVertexBuffer.Reset();
        s_Data.QuadMaterial.Reset();
        s_Data.pQuadVertexArray.Reset();
        s_Data.pWhiteTexture.Reset();
        delete[] s_Data.QuadVertexBufferBase;
    }

    void Renderer2D::BeginScene(const OrthographicCamera &camera)
    {
        BeginScene(camera.GetViewProjectionMatrix());
    }

    void Renderer2D::BeginScene(const glm::mat4& viewProj)
    {
        RendererCommand::Push<PushCameraUniformCmd>(s_Data.CameraUniformBuffer, viewProj);
        StartBatch();
    }

    void Renderer2D::BeginScene(const EditorCamera &camera)
    {
        BeginScene(camera.GetViewProjection());
    }

    void Renderer2D::EndScene()
    {
        Flush();
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return s_Data.Stats;
    }

    void Renderer2D::StartBatch()
    {
        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.CircleIndexCount = 0;
        s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

        s_Data.LineVertexCount = 0;
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::NextBatch()
    {
        EndScene();
        StartBatch();
    }

    void Renderer2D::Flush()
    {
        if (s_Data.QuadIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_Data.QuadVertexBufferPtr - (uint8_t *)s_Data.QuadVertexBufferBase;
            s_Data.pQuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

            for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i) {
                if (s_Data.TextureSlots[i])
                    s_Data.QuadMaterial->Set("u_Textures", s_Data.TextureSlots[i], i);
                else
                    s_Data.QuadMaterial->Set("u_Textures", s_Data.pWhiteTexture, i);
            }
            Renderer::RenderGeometry(s_Data.QuadMaterial, s_Data.pQuadVertexArray, s_Data.QuadIndexCount);
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.CircleIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
            s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

            Renderer::RenderGeometry(s_Data.CircleMaterial, s_Data.CircleVertexArray, s_Data.CircleIndexCount);
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.LineVertexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
            s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

            s_Data.LineShader->Bind();
            Renderer::SetLineWidth(s_Data.LineWidth);
            Renderer::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
            s_Data.Stats.DrawCalls++;
        }
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad({position.x, position.y, 0}, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawRotatedQuad(position, size, 0.f, color);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
                              const glm::vec4 &tintColor, float tilingFactor)
    {
        DrawQuad({position.x, position.y, 0}, size, texture, tintColor, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
                              const glm::vec4 &tintColor, float tilingFactor)
    {
        DrawRotatedQuad(position, size, 0.f, texture, tintColor, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture2D,
                              const glm::vec4 &tintColor, float tilingFactor)
    {
        DrawQuad({position.x, position.y, 0}, size, subTexture2D, tintColor, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture2D,
                              const glm::vec4 &tintColor, float tilingFactor)
    {
        DrawRotatedQuad(position, size, 0.f, subTexture2D, tintColor, tilingFactor);
    }


    void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                     const glm::vec4 &color)
    {
        DrawRotatedQuad({position.x, position.y, 0}, size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                     const glm::vec4 &color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});

        DrawQuad(transform, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                     const Ref<Texture2D> &texture, const glm::vec4 &tintColor, float tilingFactor)
    {
        DrawRotatedQuad({position.x, position.y, 0}, size, rotation, texture, tintColor, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                     const Ref<Texture2D> &texture, const glm::vec4 &tintColor, float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});

        DrawQuad(transform, texture, tintColor, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                     const Ref<SubTexture2D> &subTexture2D, const glm::vec4 &tintColor,
                                     float tilingFactor)
    {
        DrawRotatedQuad({position.x, position.y, 0}, size, rotation, subTexture2D, tintColor, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                     const Ref<SubTexture2D> &subTexture2D, const glm::vec4 &tintColor,
                                     float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});

        DrawQuad(transform, subTexture2D->GetTexture(), tintColor, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        DrawQuad(transform, nullptr, color, 1.0f, entityID);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor, int entityID)
    {
        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            NextBatch();

        float textureIndex = 0.0f;
        if (texture)
        {
            for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
            {
                if (*s_Data.TextureSlots[i] == *texture)
                {
                    textureIndex = (float)i;
                    break;
                }
            }

            if (textureIndex == 0.0f)
            {
                if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                    NextBatch();

                textureIndex = (float)s_Data.TextureSlotIndex;
                s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
                s_Data.TextureSlotIndex++;
            }
        }

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->oPosition = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->oColor = tintColor;
            s_Data.QuadVertexBufferPtr->oTexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->fTexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->fTilingFactor = tilingFactor;
            s_Data.QuadVertexBufferPtr->EntityID = entityID;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
    {
//        FOX_PROFILE_FUNCTION();

        // TODO: implement for circles
        // if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
        // 	NextBatch();

        for (auto& QuadVertexPosition : s_Data.QuadVertexPositions)
        {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * QuadVertexPosition;
            s_Data.CircleVertexBufferPtr->LocalPosition = QuadVertexPosition * 2.0f;
            s_Data.CircleVertexBufferPtr->Color = color;
            s_Data.CircleVertexBufferPtr->Thickness = thickness;
            s_Data.CircleVertexBufferPtr->Fade = fade;
            s_Data.CircleVertexBufferPtr->EntityID = entityID;
            s_Data.CircleVertexBufferPtr++;
        }

        s_Data.CircleIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRenderer& src, int entityID)
    {
        if (AssetManager::IsAssetHandleValid(src.Sprite))
            DrawQuad(transform, AssetManager::GetAsset<Texture2D>(src.Sprite), src.Color, src.TilingFactor, entityID);
        else
            DrawQuad(transform, src.Color, entityID);
    }

    void Renderer2D::DrawParticle(const glm::mat4& transform, ParticleSystem& src)
    {
        auto activeParticle = iter(src.ParticlePool) | filter([](const Particle& particle) { return particle.Active; });
        while (auto particle = activeParticle.next())
        {
            DrawRotatedQuad(particle->Position, particle->Size, particle->Rotation, particle->Color);
        }
    }

    void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
    {
        s_Data.LineVertexBufferPtr->Position = p0;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexBufferPtr->Position = p1;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexCount += 2;
    }

    void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
    {
        glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
        glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

        DrawLine(p0, p1, color, entityID);
        DrawLine(p1, p2, color, entityID);
        DrawLine(p2, p3, color, entityID);
        DrawLine(p3, p0, color, entityID);
    }

    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
        glm::vec3 lineVertices[4];
        for (size_t i = 0; i < 4; i++)
            lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

        DrawLine(lineVertices[0], lineVertices[1], color, entityID);
        DrawLine(lineVertices[1], lineVertices[2], color, entityID);
        DrawLine(lineVertices[2], lineVertices[3], color, entityID);
        DrawLine(lineVertices[3], lineVertices[0], color, entityID);
    }

    float Renderer2D::GetLineWidth()
    {
        return s_Data.LineWidth;
    }

    void Renderer2D::SetLineWidth(float width)
    {
        s_Data.LineWidth = width;
    }
}
