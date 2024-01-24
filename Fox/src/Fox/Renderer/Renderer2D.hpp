//
// Created by samuel on 28/06/2021.
//

#ifndef FOXENGINE_RENDERER2D_HPP
#define FOXENGINE_RENDERER2D_HPP

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#include "common.hpp"

#include <queue>
#include <variant>
#include <string>
#include <iostream>

namespace fox
{
    class QuadVertex;
    class VertexArray;
    class Shader;
    struct SpriteRenderer;
    struct ParticleSystem;
    class Texture2D;
    class SubTexture2D;
    class Camera;

    class Material;
    class Mesh;

    class Renderer2D
    {
    public:
        struct CameraData
        {
            glm::mat4 View;
            glm::mat4 Projection;
            glm::mat4 ViewProjection;
            glm::vec3 CamPos;
        };

        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera& camera);
        static void EndScene();
        static void Flush();

        static Ref<Material> GetDefaultSpriteMaterial();

        static void DrawMesh(Ref<Material>& material, Ref<Mesh>& mesh);

        // Primitives
        //static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture2D, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
        //static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f, int entityID = -1, Ref<Material> material = nullptr);

        //static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture2D, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);

        static void DrawSprite(const glm::mat4& transform, SpriteRenderer& src, int entityID);
        static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

        static void DrawParticle(const glm::mat4& transform, ParticleSystem& src);

        static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static float GetLineWidth();
        static void SetLineWidth(float width);

        static glm::mat4 MakeMat4(glm::vec3 position, glm::vec3 scale = glm::vec3(1), glm::vec3 rotation = glm::vec3(0));
        static glm::mat4 MakeMat4(glm::vec3 position, glm::vec3 scale = glm::vec3(1), float rotation = 0);

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
