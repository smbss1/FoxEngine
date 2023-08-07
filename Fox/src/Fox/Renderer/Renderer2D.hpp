//
// Created by samuel on 28/06/2021.
//

#ifndef FOXENGINE_RENDERER2D_HPP
#define FOXENGINE_RENDERER2D_HPP

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

namespace fox
{
    class QuadVertex;
    class Shader;
    struct SpriteRenderer;
    struct ParticleSystem;
    struct Texture2D;
    struct SubTexture2D;
    struct EditorCamera;
    struct OrthographicCamera;

    class Renderer2D
    {
    public:
        struct CameraData
        {
            glm::mat4 ViewProjection;
        };

        static void Init();
        static void Shutdown();

        static void BeginScene(const glm::mat4& viewProj);
        static void BeginScene(const EditorCamera& camera);
        static void BeginScene(const OrthographicCamera& camera); // TODO: Remove
        static void EndScene();
        static void Flush();

        // Primitives
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture2D, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture2D, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f, int entityID = -1);

        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture2D, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture2D, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

        static void DrawSprite(const glm::mat4& transform, SpriteRenderer& src, int entityID);
        static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

        static void DrawParticle(const glm::mat4& transform, ParticleSystem& src);

        static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static float GetLineWidth();
        static void SetLineWidth(float width);

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            [[nodiscard]] uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            [[nodiscard]] uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };
        static void ResetStats();
        static Statistics GetStats();

    private:
        static void StartBatch();
        static void NextBatch();
    };
}

#endif //FOXENGINE_RENDERER2D_HPP
