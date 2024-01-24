//
// Created by samuel on 28/06/2021.
//

#include "common.hpp"
#include "Renderer2D.hpp"
#include "UniformBuffer.hpp"
#include "Asset/AssetManager.hpp"
#include "Renderer.hpp"
#include "EditorCamera.hpp"
#include "../../../../FoxEditor/src/EditorLayer.hpp"
#include "Commands.hpp"
#include "Utils/Iterator.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "RendererCommand.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Utils/Buffer.hpp"
#include "Utils/MemoryBuffer.hpp"
#include "Utils/expected.hpp"

#include "RenderQueue.hpp"

namespace fox
{
    struct DrawPair
    {
        Ref<Material> material;
        Ref<Mesh> mesh;
        uint32_t instanceCount = 1;
    };

    RenderCommandQueue m_RenderQueue;
    static std::unordered_map<uint32_t, DrawPair> m_Draws;

    struct QuadVertex
    {
        glm::vec4 Color;
        float TexIndex;
        float TilingFactor;

        // Editor only
        int EntityID;
        glm::mat4 ModelMatrice;
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
        static const uint32_t  MaxTextureSlots = 32; // TODO: RenderCaps

        Ref<Mesh> Quad;

        Ref<VertexBufferPool> QuadVertexBufferPool;
        Ref<Material> QuadMaterial;
        Ref<Texture2D> WhiteTexture;

        Ref<VertexArray> CircleVertexArray;
        Ref<VertexBuffer> CircleVertexBuffer;
        Ref<Material> CircleMaterial;

        Ref<VertexArray> LineVertexArray;
        Ref<VertexBuffer> LineVertexBuffer;
        Ref<Shader> LineShader;

        uint32_t QuadInstanceCount = 0;

        uint32_t CircleIndexCount = 0;
        CircleVertex* CircleVertexBufferBase = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;

        uint32_t LineVertexCount = 0;
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;
        float LineWidth = 2.0f;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        glm::vec3 QuadVertexPositions[4];

        Renderer2D::Statistics Stats;

        Renderer2D::CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;

        Ref<Shader> PointLightShader;
    };

    static Renderer2DData s_Data;

    struct RenderNonInstancedCommand
    {
        Ref<VertexArray> vao;
        Ref<Material> material;
        uint32_t indices;
        uint32_t indexType;

        //static void Execute()
        //{
        //    s_Data.QuadVertexBufferPool->Bind();

        //    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i) {
        //        material->Set("u_Textures", s_Data.TextureSlots[i], i);
        //    }
        //    ////s_Data.QuadMaterial->Set("lightPos", glm::vec3(0, 0, 2));
        //    Renderer::RenderInstanceGeometry(renderCmd.material, s_Data.Quad, renderCmd.instanceCount);

        //    s_Data.Stats.DrawCalls++;
        //}
    };

    struct RenderInstancedCommand
    {
        Mesh* mesh;
        VertexArray* vao;
        Material* material;
        uint32_t indices;
        uint32_t indexType;
        uint32_t instanceCount;
        size_t instanceDataPoolId;

        static void Execute(const void* data)
        {
            auto& cmd = *reinterpret_cast<const RenderInstancedCommand*>(data);

            //s_Data.QuadVertexBufferPool->Bind();

            for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i) {
                cmd.material->Set("u_Textures", s_Data.TextureSlots[i], i);
            }
            //FOX_INFO("%", cmd.instanceCount);
            ////s_Data.QuadMaterial->Set("lightPos", glm::vec3(0, 0, 2));
            Renderer::RenderInstanceGeometry(cmd.material, cmd.mesh, cmd.instanceCount);

            s_Data.Stats.DrawCalls++;
        }
    };

    void Renderer2D::Init()
    {
        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f };
        s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f };
        s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f };
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f };

        std::vector<uint32_t> quadIndices = std::vector<uint32_t>(6);

        uint32_t offset = 0;
        for (int i = 0; i < 6; i += 6) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
            offset += 4;
        }
        auto quadIB = IndexBuffer::Create(quadIndices.data(), quadIndices.size());

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        std::vector<Vertex> vertices = {
            Vertex{  s_Data.QuadVertexPositions[0], glm::vec3(0, 0, 1), textureCoords[0] },
            Vertex{  s_Data.QuadVertexPositions[1], glm::vec3(0, 0, 1), textureCoords[1] },
            Vertex{  s_Data.QuadVertexPositions[2], glm::vec3(0, 0, 1), textureCoords[2] },
            Vertex{  s_Data.QuadVertexPositions[3], glm::vec3(0, 0, 1), textureCoords[3] },
        };

        // Quad
        s_Data.Quad = new_ref<Mesh>(vertices, quadIndices);
        s_Data.QuadVertexBufferPool = VertexBufferPool::Create();
        s_Data.QuadVertexBufferPool->Initialize(sizeof(QuadVertex), s_Data.MaxQuads, true);
        s_Data.QuadVertexBufferPool->SetLayout({
              {3, fox::ShaderDataType::Float4, "a_Color", 1},
              {4, fox::ShaderDataType::Float,  "a_TexIndex", 1},
              {5, fox::ShaderDataType::Float,  "a_TilingFactor", 1},
              {6, fox::ShaderDataType::Int,    "a_EntityID", 1},
              {7, fox::ShaderDataType::Mat4,   "a_ModelMatrice", 1},
        }, s_Data.MaxQuads);
        s_Data.Quad->VertexArray->AddVertexBuffer(s_Data.QuadVertexBufferPool);


        // Circles
        s_Data.CircleVertexArray = VertexArray::Create();
        s_Data.CircleVertexBuffer = VertexBuffer::Create();
        s_Data.CircleVertexBuffer->Initialize(s_Data.MaxVertices * sizeof(CircleVertex), 1, true);
        s_Data.CircleVertexBuffer->SetLayout({
            {0, fox::ShaderDataType::Float3,   "a_WorldPosition"},
            {1, fox::ShaderDataType::Float3,   "a_LocalPosition"},
            {2, fox::ShaderDataType::Float4,   "a_Color"},
            {3, fox::ShaderDataType::Float,    "a_Thickness"},
            {4, fox::ShaderDataType::Float,    "a_Fade"},
            {5, fox::ShaderDataType::Int,      "a_EntityID"},
        });
        s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
        s_Data.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
        s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

        // Lines
        s_Data.LineVertexArray = VertexArray::Create();
        s_Data.LineVertexBuffer = VertexBuffer::Create();
        s_Data.LineVertexBuffer->Initialize(s_Data.MaxVertices * sizeof(LineVertex), 1, true);
        s_Data.LineVertexBuffer->SetLayout({
           { 0, ShaderDataType::Float3, "a_Position" },
           { 1, ShaderDataType::Float4, "a_Color"    },
           { 2, ShaderDataType::Int,    "a_EntityID" },
        });
        s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];


        s_Data.WhiteTexture = Texture2D::Create(TextureSpecification());
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(Buffer(&whiteTextureData, sizeof(uint32_t)));

        s_Data.QuadMaterial = new_ref<Material>(Shader::Create("assets/shaders/Renderer2D_Quad.glsl"));
        s_Data.CircleMaterial = new_ref<Material>(Shader::Create("assets/shaders/Renderer2D_Circle.glsl"));
        s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");
        //s_Data.PointLightShader = Shader::Create("assets/shaders/light.glsl");

        // Set first texture slot to 0
        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2D::CameraData), 0);
    }

    void Renderer2D::Shutdown()
    {
        m_Draws.clear();
        m_RenderQueue.Destroy();

        for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; ++i)
            s_Data.TextureSlots[i] = nullptr;
        s_Data.QuadVertexBufferPool = nullptr;
        s_Data.QuadMaterial = nullptr;
        s_Data.Quad = nullptr;
        s_Data.WhiteTexture = nullptr;

        s_Data.CircleMaterial = nullptr;
        s_Data.CircleVertexBuffer = nullptr;
        s_Data.CircleVertexArray = nullptr;

        s_Data.LineVertexArray = nullptr;
        s_Data.LineVertexBuffer = nullptr;
        s_Data.LineShader = nullptr;
        
        s_Data.CameraUniformBuffer = nullptr;
        s_Data.PointLightShader = nullptr;
    }

    void Renderer2D::BeginScene(const Camera& camera)
    {
        Renderer2D::CameraData data;
        data.View = camera.GetViewMatrix();
        data.Projection = camera.GetProjectionMatrix();
        data.ViewProjection = camera.GetViewProjectionMatrix();
        data.CamPos = camera.GetPosition();
        RendererCommand::Push<PushCameraUniformCmd>(s_Data.CameraUniformBuffer, data);
        StartBatch();
    }

    void Renderer2D::EndScene()
    {
        Flush();
        m_RenderQueue.Execute();
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
        s_Data.QuadInstanceCount = 0;

        s_Data.CircleIndexCount = 0;
        s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

        s_Data.LineVertexCount = 0;
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

        s_Data.TextureSlotIndex = 1;

        //m_Draws.clear();
        for (auto& it : m_Draws)
        {
            it.second.instanceCount = 0;
        }
    }

    Ref<Material> Renderer2D::GetDefaultSpriteMaterial()
    {
        return s_Data.QuadMaterial;
    }

    void Renderer2D::NextBatch()
    {
        EndScene();
        StartBatch();
    }

//#define TRY(variable, operation) \
//        auto&& _temp_res = (operation); \
//        if (!_temp_res.has_value()) { \
//            return _temp_res; \
//        } \
//        auto&& variable = _temp_res.value();
//
//    tl::expected<int, std::string> mightFail(bool fail) {
//        if (fail) {
//            return tl::make_unexpected<std::string>("Failed");
//        }
//        return 42;
//    }
//
//    tl::expected<int, std::string> process() {
//        TRY(value, mightFail(false));
//        return value;
//    }

    void Renderer2D::Flush()
    {
        if (m_Draws.size())
        {
            /*std::vector<std::pair<uint32_t, DrawPair>> elems(m_Draws.begin(), m_Draws.end());
            std::sort(elems.begin(), elems.end(), [](std::pair<uint32_t, DrawPair> a, std::pair<uint32_t, DrawPair> b) {
                return a.second.material->GetID() < b.second.material->GetID();
            });*/

            for (auto& it : m_Draws)
            {
                if (it.second.instanceCount == 0)
                    continue;

                RenderInstancedCommand& cmd = *m_RenderQueue.Push<RenderInstancedCommand>();
                cmd.mesh = it.second.mesh.Raw();
                cmd.vao = it.second.mesh->VertexArray.Raw();
                cmd.material = it.second.material.Raw();
                cmd.indices = it.second.mesh->VertexArray->GetIndexBuffer()->GetCount();
                cmd.indexType = 0x1405; // unsign int
                cmd.instanceDataPoolId = it.second.material->GetID();
                cmd.instanceCount = it.second.instanceCount;
            }
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

            RendererCommand::Push<FunctionCmd>([shader = s_Data.LineShader, VertexArray = s_Data.LineVertexArray, Count = s_Data.LineVertexCount]() mutable {
                shader->Bind();
                Renderer::SetLineWidth(s_Data.LineWidth);
                Renderer::DrawLines(VertexArray, Count);
            });
            s_Data.Stats.DrawCalls++;
        }
    }

    //void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture2D,
    //                          const glm::vec4 &tintColor, float tilingFactor)
    //{
    //    DrawRotatedQuad(position, size, 0.f, subTexture2D, tintColor, tilingFactor);
    //}

    //void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
    //                                 const Ref<SubTexture2D> &subTexture2D, const glm::vec4 &tintColor,
    //                                 float tilingFactor)
    //{
    //    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
    //                          glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
    //                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.0f});

    //    DrawQuad(transform, subTexture2D->GetTexture(), tintColor, tilingFactor);
    //}

    //void addPaddingForAlignment(std::vector<uint8_t>& buffer, size_t alignment = 16) {
    //    size_t paddingSize = (alignment - (buffer.size() % alignment)) % alignment;
    //    buffer.insert(buffer.end(), paddingSize, 0);
    //}

    //void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor, int entityID, Ref<Material> material)
    //{
    //    //if (s_Data.QuadInstanceCount >= Renderer2DData::MaxQuads)
    //    //    NextBatch();

    //    //float textureIndex = 0.0f;
    //    //if (texture)
    //    //{
    //    //    // Find the texture
    //    //    for (uint32_t i = 1; i < s_Data.TextureSlotIndex; ++i)
    //    //    {
    //    //        if (*s_Data.TextureSlots[i] == *texture)
    //    //        {
    //    //            textureIndex = (float)i;
    //    //            break;
    //    //        }
    //    //    }

    //    //    // if not found, add to the texture slot
    //    //    if (textureIndex == 0.0f)
    //    //    {
    //    //        if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
    //    //            NextBatch();

    //    //        textureIndex = (float)s_Data.TextureSlotIndex;
    //    //        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
    //    //        s_Data.TextureSlotIndex++;
    //    //    }
    //    //}

    //    ////DrawMesh(glm::mat4(1.0f), nullptr, 0);

    //    //MemoryBuffer buffer;

    //    //buffer.write<glm::vec4>(tintColor);
    //    //buffer.write<float>(textureIndex);
    //    //buffer.write<float>(tilingFactor);
    //    //buffer.write<int>(entityID);
    //    //buffer.write<glm::mat4>(transform);

    //    //RenderCommand cmd;
    //    //cmd.vao = s_Data.Quad->VertexArray;
    //    //cmd.indices = s_Data.Quad->VertexArray->GetIndexBuffer()->GetCount();
    //    //cmd.indexType = 0x1405; // unsign int
    //    //cmd.instanceData = std::move(buffer);
    //    //cmd.material = s_Data.QuadMaterial;
    //    //cmd.instanceCount = 1;
    //    ////s_Data.QuadMaterial->Set("u_Textures", s_Data.TextureSlots[textureIndex], (uint32_t)textureIndex);

    //    //m_RenderQueue.push_back(cmd);

    //    ////s_Data.QuadVertexBufferBaseVec.write<glm::vec4>(tintColor);
    //    ////s_Data.QuadVertexBufferBaseVec.write<float>(textureIndex);
    //    ////s_Data.QuadVertexBufferBaseVec.write<float>(tilingFactor);
    //    ////s_Data.QuadVertexBufferBaseVec.write<int>(entityID);
    //    ////s_Data.QuadVertexBufferBaseVec.write<glm::mat4>(transform);

    //    ////s_Data.QuadVertexBufferPtr->Color = tintColor;
    //    ////s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    //    ////s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    //    ////s_Data.QuadVertexBufferPtr->EntityID = entityID;
    //    ////s_Data.QuadVertexBufferPtr->ModelMatrice = transform;
    //    ////s_Data.QuadVertexBufferPtr++;

    //    //s_Data.QuadInstanceCount++;
    //    //s_Data.Stats.QuadCount++;
    //}

    void Renderer2D::DrawMesh(Ref<Material>& material, Ref<Mesh>& mesh)
    {
        if (s_Data.QuadInstanceCount >= Renderer2DData::MaxQuads)
            NextBatch();

        float textureIndex = 0.0f;
        if (material->GetMainTexture())
        {
            // Find the texture
            for (uint32_t i = 1; i < s_Data.TextureSlotIndex; ++i)
            {
                if (*s_Data.TextureSlots[i] == *material->GetMainTexture())
                {
                    textureIndex = (float)i;
                    break;
                }
            }

            // if not found, add to the texture slot
            if (textureIndex == 0.0f)
            {
                if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                    NextBatch();

                textureIndex = (float)s_Data.TextureSlotIndex;
                s_Data.TextureSlots[s_Data.TextureSlotIndex] = material->GetMainTexture();
                s_Data.TextureSlotIndex++;
            }
        }

        material->SetAttribute(1, textureIndex);
        material->UpdateBuffer();

        int instanceIndex = 0;
        auto& it = m_Draws[material->GetID()];
        it.material = material;
        it.mesh = mesh;
        it.instanceCount++;
        instanceIndex = it.instanceCount - 1;

        s_Data.QuadVertexBufferPool->SetSubData(instanceIndex, material->GetAttributesData().buffer(), material->GetAttributesData().size());

        s_Data.QuadInstanceCount++;
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
            s_Data.CircleVertexBufferPtr->WorldPosition = QuadVertexPosition;
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
        Ref<Material> material = src.Material;
        material->SetAttribute(0, src.Color);
        material->SetAttribute(1, 0.0f); // update later in DrawMesh function
        material->SetAttribute(2, src.TilingFactor);
        material->SetAttribute(3, entityID);
        material->SetAttribute(4, transform);
        material->SetMainTexture(AssetManager::GetAsset<Texture2D>(src.Sprite));
        DrawMesh(material, s_Data.Quad);
    }

    void Renderer2D::DrawParticle(const glm::mat4& transform, ParticleSystem& src)
    {
        Ref<Material> material = src.Material;
        material->SetAttribute(1, 0.0f); // update later in DrawMesh function
        material->SetAttribute(2, 1.0f);
        material->SetAttribute(3, -1);

        for (const auto& particle : reverse(src.ParticlePool))
        {
            if (particle.Active)
            {
                glm::vec3 pos = { particle.Position.x, particle.Position.y, particle.Depth };
                material->SetAttribute(0, particle.Color);
                material->SetAttribute(4, Renderer2D::MakeMat4(pos, glm::vec3(particle.Size.x, particle.Size.y, 0), particle.Rotation));
                DrawMesh(material, s_Data.Quad);
            }
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
            lineVertices[i] = transform * glm::vec4(s_Data.QuadVertexPositions[i], 1.0f);

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

    glm::mat4 Renderer2D::MakeMat4(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
    {
        glm::mat4 rot = glm::toMat4(glm::quat(rotation));
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * rot
            * glm::scale(glm::mat4(1.0f), scale);

        return transform;
    }

    glm::mat4 Renderer2D::MakeMat4(glm::vec3 position, glm::vec3 scale, float rotation)
    {
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * rot
            * glm::scale(glm::mat4(1.0f), scale);

        return transform;
    }
}
