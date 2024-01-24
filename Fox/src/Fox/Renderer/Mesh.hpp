
#ifndef FOXENGINE_MESH_
#define FOXENGINE_MESH_

#include <vector>
#include <glm/glm.hpp>
#include "Core/Ref.hpp"

namespace fox
{
    class VertexBuffer;
    class VertexArray;
    class IndexBuffer;
    class Material;

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    };

	struct Mesh : public RefCounted
	{
        Ref<VertexArray> VertexArray;
        Ref<VertexBuffer> VertexBuffer;
        Ref<Material> Material;

        Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices = {});
	};
}

#endif