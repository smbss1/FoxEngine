

#include <Core/Input/InputManager.hpp>
#include <Core/Logger/Logger.hpp>
#include <Core/State.hpp>
#include <Core/Managers/StateMachine.hpp>

#include <Renderer/Renderer.hpp>
#include <Renderer/RendererCommand.hpp>
#include <Renderer/Buffer.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Shader.hpp>

#include "SandboxApp.hpp"
#include "ScriptableBehaviour.hpp"
#include "NativeScript.hpp"
#include "Animator.hpp"

#include <glm/gtc/matrix_transform.hpp>

class Test : public ScriptableBehaviour
{
    float value;
public:
	virtual ~Test() {}

protected:
	virtual void on_create() override
	{
		std::cout << "OnCreate!" << std::endl;

        add_component<fox::Transform>(Vec2(45, 25));
        add_component<SpriteRenderer>("", "MySprite", 12);
        auto& anim = add_component<Animator>().add_anim_from_file("sandbox/assets/animation.json");
        anim.signal(4, [&]{ fox::info("Value: %", value); });
        anim.signal(4.1, [&]{ fox::info("Value: %", value); });
        anim.signal(4.5, [&]{ fox::info("Value: %", value); });
        anim.signal(5.00001, [&]{ fox::info("Value: %", value); });
        anim.signal(6.0001, [&]{ fox::info("Value: %", value); });
        anim.signal(7.001, [&]{ fox::info("Value: %", value); });
        anim.signal(8.01, [&]{ fox::info("Value: %", value); });
        anim.get<float>(0).apply(value);
	}

	virtual void on_update() override
	{
		// std::cout << "OnUpdate!"  << i++ << std::endl;
        fox::InputManager& input = get_app().get<fox::InputManager>().value();

        if (input.GetKeyDown(fox::Key::A))
        {
            fox::info("A pressed");
        }

        if (input.GetMouseButtonDown(fox::MouseButton::ButtonLeft))
        {
            fox::info("Left pressed");
        }

        if (input.GetMouseButtonUp(fox::MouseButton::ButtonLeft))
        {
            fox::info("Left released");
        }

        if (input.GetMouseButtonDown(fox::MouseButton::ButtonRight))
        {
            fox::info("Right pressed");
        }
//        fox::info("%", input.GetMousePosition());
//        fox::info("Scroll: %", input.GetMouseScroll());
	}

	virtual void on_destroy() override { }
    int i = 0;
};

class ExampleScene : public fox::State
{
    fox::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPos;
    float m_CameraRot;
    float m_fCameraMoveSpeed = 10.0f;
    float m_fCameraRotSpeed = 500.0f;

    glm::vec3 m_TrianglePos;
    float m_fTriangleMoveSpeed = 10.0f;

    fox::ref<fox::VertexArray> va;
    fox::ref<fox::Shader> shader;

public:
    ExampleScene()
    : m_Camera(-1.0f, 1.0f, -1.0f, 1.0f)
    , fox::State("ExampleScene") { }

    ~ExampleScene() = default;

    void OnEnter() override
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

        shader = fox::Shader::Create("plugins/SFMLPlugin/assets/shaders/Basic.shader");
        shader->Bind();
    }

    void OnExit() override
    {}

    void OnUpdate() override
    {
        auto& inputManager = GetApp().get<fox::InputManager>().value();
        if (inputManager.GetKeyDown(fox::Key::Left))
            m_CameraPos.x += m_fCameraMoveSpeed * Time::delta_time;
        else if (inputManager.GetKeyDown(fox::Key::Right))
            m_CameraPos.x -= m_fCameraMoveSpeed * Time::delta_time;
        if (inputManager.GetKeyDown(fox::Key::Up))
            m_CameraPos.y -= m_fCameraMoveSpeed * Time::delta_time;
        else if (inputManager.GetKeyDown(fox::Key::Down))
            m_CameraPos.y += m_fCameraMoveSpeed * Time::delta_time;

        if (inputManager.GetKey(fox::Key::A))
            m_CameraRot -= m_fCameraRotSpeed * Time::delta_time;
        else if (inputManager.GetKey(fox::Key::E))
            m_CameraRot += m_fCameraRotSpeed * Time::delta_time;

        if (inputManager.GetKeyDown(fox::Key::J))
            m_TrianglePos.x -= m_fCameraMoveSpeed * Time::delta_time;
        else if (inputManager.GetKeyDown(fox::Key::L))
            m_TrianglePos.x += m_fCameraMoveSpeed * Time::delta_time;
        if (inputManager.GetKeyDown(fox::Key::I))
            m_TrianglePos.y += m_fCameraMoveSpeed * Time::delta_time;
        else if (inputManager.GetKeyDown(fox::Key::K))
            m_TrianglePos.y -= m_fCameraMoveSpeed * Time::delta_time;

        m_Camera.SetPosition(m_CameraPos);
        m_Camera.SetRotation(m_CameraRot);

        fox::RendererCommand::SetClearColor({0, 0, 0, 1});
        fox::RendererCommand::Clear();

        fox::Renderer::BeginScene(m_Camera);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_TrianglePos);
        fox::Renderer::Submit(shader, va, transform);
        fox::Renderer::EndScene();
    }
};

SandboxApp::SandboxApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

SandboxApp::~SandboxApp() { }

void SandboxApp::init()
{
    fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
    sceneManager.PushState(new ExampleScene);
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new SandboxApp(argc, argv);
}