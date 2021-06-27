

#include <Core/Managers/PluginManager.hpp>
#include <chrono>
#include <Time.hpp>
#include <Utils/FileSystem.hpp>
#include <Core/Managers/StateMachine.hpp>
#include <Core/Managers/ResourceManager.hpp>
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommand.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Buffer.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Shader.hpp>
#include "Plugin/IGraphic.hpp"
#include "Core/Input/InputManager.hpp"
#include "Core/Application.hpp"
#include "Core/Logger/Logger.hpp"
#include "json.hpp"

namespace fox
{
    Application::Application(int ac, char** av)
    {
        m_bIsRunning = true;
        set<StateMachine>(*this);
        set<ResourceManager>();
        set<InputManager>();
        set<TimeInfo>();
        m_pWorld = new_scope<World>();

        get_world().add_phase(game::OnStart);
        get_world().add_phase(ecs::OnSceneEnable);
        get_world().add_phase(ecs::OnSceneDisable);
        get_world().add_phase(ecs::PostFixUpdate);
        get_world().add_phase(ecs::PreFixUpdate);
        get_world().add_phase(ecs::OnAddScript);
    }

    Application::~Application()
    {
    }

    void Application::init()
    {
    }

    void Application::quit()
    {
        m_bIsRunning = false;
    }

    void Application::LoadConfig()
    {
        fox::info("Starting load config.json....");
        json::Value oConfigTemp;
        std::string out;
        if (fox::file::ReadFile("config.json", out))
            oConfigTemp = json::parse(out);
        else
            fox::error("Cannot read config.json");
        if (!oConfigTemp.is_null()) {
            m_oConfigFile = new_scope<json::Value>(oConfigTemp);
            fox::info("config.json load successfully");
        }
        else
            fox::error("Wrong configuration format for 'config.json'");
    }

    void Application::run()
    {
        fox::info("Starting Application...");
        float fFixedDeltaTime = 0;
        float fFixedTimeStep = 1.0f / 45.0f;
        float fDeltaTime;
        auto lastUpdate = std::chrono::high_resolution_clock::now();

        LoadConfig();

        PluginManager& plugin_manager = get_or_create<fox::PluginManager>().value();
        plugin_manager.FindAndLoadPlugins((*m_oConfigFile)["plugins directory"].get<std::string>());
        if(plugin_manager.GetGraphics().GetCount() <= 0)
            throw std::runtime_error("Cannot run the application because no Graphics Plugins found");

        plugin_manager.InitializePlugins(*this);
        GraphicPlugin& graphic_ctx = plugin_manager.GetGraphics().GetPlugin(0);
        RendererAPI::SetGraphicPlugin(&graphic_ctx);
        RendererCommand::SetRendererAPI(graphic_ctx.CreateRenderer());


        float vertices[3 * 7] = {
                -0.5f,  -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, // 0
                0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, // 1
                0.5f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f // 2
        };

        unsigned int indices[] = {
                0, 1, 2,
        };

        auto va = graphic_ctx.CreateVertexArray();
        auto vb = graphic_ctx.CreateVertexBuffer(vertices, sizeof(vertices));

        BufferLayout layout = {
//                {ShaderDataType::Float2, "position"},
//                {ShaderDataType::Float2, "texCoords"}

                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4, "a_Color"}
        };

        vb->SetLayout(layout);
        va->AddVertexBuffer(vb);

        auto ib = graphic_ctx.CreateIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t));
        va->SetIndexBuffer(ib);

        ref<Shader> shader = graphic_ctx.CreateShader("plugins/SFMLPlugin/assets/shaders/Basic.shader");
        shader->Bind();

//        auto texture = graphic_ctx.create_texture("_fail_.png");
//        texture->Bind();

//        shader->SetUniform("u_Texture", 0);

        init();
        fox::info("Application is running");
        while (m_bIsRunning)
        {
            graphic_ctx.poll_event();

            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = now - lastUpdate;
            lastUpdate = now;

            fDeltaTime = elapsed.count();
            Time::delta_time = fDeltaTime;
            if (Time::delta_time > 0.25)
                Time::delta_time = 0.25;
            Time::time += fDeltaTime;
            get<TimeInfo>()->delta_time = Time::delta_time;
            get<TimeInfo>()->time = Time::time;

            fFixedDeltaTime += fDeltaTime;
            while (fFixedDeltaTime >= fFixedTimeStep)
            {
                // get<SceneManager>()->fix_update();
                Time::fixed_delta_time = fFixedDeltaTime;
                get<TimeInfo>()->time = Time::fixed_delta_time;
                fFixedDeltaTime -= fFixedTimeStep;
            }
            Time::factor_physics = fFixedDeltaTime / fFixedTimeStep;
            get<TimeInfo>()->factor_physics = Time::factor_physics;

            get<StateMachine>()->Update();

            RendererCommand::SetClearColor({0, 0, 0, 1});
            RendererCommand::Clear();

            Renderer::BeginScene();

            shader->Bind();
            Renderer::Submit(va);
            Renderer::EndScene();

            graphic_ctx.update();
        }
        m_pWorld.reset();
        remove<ResourceManager>();
    }
}
