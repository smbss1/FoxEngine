//
// Created by samuel on 28/06/2021.
//

#include "FirstTriangle.hpp"

void FirstTriangleState::OnEnter()
{
    //        GetWorld().new_entity().add<NativeScript>(Test());

    float vertices[3 * 7] = {
            -0.5f,  -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, // 0
            0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, // 1
            0.5f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f // 2
    };

    unsigned int indices[] = {
            0, 1, 2,
    };

    va = fox::VertexArray::Create();
    auto vb = fox::VertexBuffer::Create(vertices, sizeof(vertices));

    fox::BufferLayout layout = {
    //                {ShaderDataType::Float2, "position"},
    //                {ShaderDataType::Float2, "texCoords"}

            {fox::ShaderDataType::Float3, "a_Position"},
            {fox::ShaderDataType::Float4, "a_Color"}
    };

    vb->SetLayout(layout);
    va->AddVertexBuffer(vb);

    auto ib = fox::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    va->SetIndexBuffer(ib);

    shader = fox::Shader::Create("assets/shaders/Basic.glsl");
    shader->Bind();
}

void FirstTriangleState::OnExit()
{}

void FirstTriangleState::OnEvent(fox::Event& event)
{
    m_Camera.OnEvent(event);
}

void FirstTriangleState::OnUpdate()
{
    m_Camera.OnUpdate();

    fox::RendererCommand::SetClearColor({0, 0, 0, 1});
    fox::RendererCommand::Clear();

    fox::Renderer::BeginScene(m_Camera.GetCamera());

    fox::Renderer::Submit(shader, va);
    fox::Renderer::EndScene();
}