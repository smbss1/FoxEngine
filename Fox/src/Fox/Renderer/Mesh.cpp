#include "Mesh.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Material.hpp"

namespace fox
{
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
	{
        VertexArray = VertexArray::Create();
        VertexBuffer = VertexBuffer::Create();
        VertexBuffer->Initialize(sizeof(Vertex), vertices.size(), false, vertices.data());
        //VertexBuffer->SetData(vertices.data(), vertices.size() * sizeof(Vertex));
        VertexBuffer->SetLayout({
              {0, fox::ShaderDataType::Float3, "a_Position"},
              {1, fox::ShaderDataType::Float3, "a_Normal"},
              {2, fox::ShaderDataType::Float2, "a_TexCoord"},
        });
        VertexArray->AddVertexBuffer(VertexBuffer);

        if (!indices.empty())
        {
            auto quadIB = IndexBuffer::Create(indices.data(), indices.size());
            VertexArray->SetIndexBuffer(quadIB);
        }
        VertexArray->Unbind();
    }
}