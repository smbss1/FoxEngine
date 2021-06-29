/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include <Core/Managers/PluginManager.hpp>
#include <Components/EntityName.hpp>
#include <Core/Managers/StateMachine.hpp>
#include <ImGui/imgui.h>
#include <glad/glad.h>
#include <Time.hpp>
#include <fstream>
#include <OpenGLShader.hpp>
#include <OpenGLTexture.hpp>
#include <ImGui/imgui_impl_opengl3.h>
#include <OpenGLRendererAPI.hpp>
#include <OpenGLContext.hpp>
#include <ImGui/imgui_impl_glfw.h>
#include "GlfwWindow.hpp"
#include "Core/Input/InputManager.hpp"
#include "Core/Managers/ResourceManager.hpp"
#include "TextureManager.hpp"
#include "SpriteManager.hpp"
#include "Components.hpp"
#include "PluginEntry.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace fox
{
    OpenGLRendererPlugin::~OpenGLRendererPlugin()
    {
    }

    void OpenGLRendererPlugin::plug(Application &app)
    {
        m_oApp = &app;
        std::cout << "[OpenGLRendererPlugin] Init!" << std::endl;


        // Stuff for init ImGui in SFML
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

//        io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
//        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

//        SetDarkThemeColors();

        auto* window = static_cast<GLFWwindow*>(app.GetWindow()->GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");


//        ResourceManager& resource_manager = app.get<ResourceManager>().value();
//        resource_manager.AddManager<sf::Texture>(new TextureManager(resource_manager));
//        resource_manager.AddManager<sf::Sprite>(new SpriteManager(resource_manager));
//
//        app.get_world().system<SpriteRenderer>("InitSpriteRenderer")
//            .kind(ecs::OnAdd).each([&](Entity& e, SpriteRenderer& renderer)
//               {
//                   resource_manager.AddAsset<sf::Texture>(renderer.m_strId, renderer.m_strPath);
//                   resource_manager.AddAsset<sf::Sprite>(renderer.m_strId, renderer.m_strPath);
//               });
//
//        app.get_world().system<Transform, SpriteRenderer>("DrawSpriteRenderer")
//            .order_by([](Transform& t1, SpriteRenderer& r1, Transform& t2, SpriteRenderer& r2)
//              {
//                  return r1.depth < r2.depth;
//              })
//            .kind(ecs::OnStore).each([&](Entity& e, Transform& transform, SpriteRenderer& renderer)
//                 {
//                     auto* sprite = resource_manager.GetAsset<sf::Sprite>(renderer.m_strId);
//                     sprite->setPosition(transform.position.x, transform.position.y);
//                     m_RenderTexture->setView(*m_EditorCamera);
//                     if (m_RenderTexture)
//                         m_RenderTexture->draw(*sprite);
//                     else
//                        m_oWindow.draw(*sprite);
//                 });
    }

    void OpenGLRendererPlugin::unplug(Application &app)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void OpenGLRendererPlugin::update()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
//        ImGuizmo::BeginFrame();

//        m_oApp->get_world().run_phase(ecs::OnStore);
//
//        m_oApp->get<fox::StateMachine>()->UpdateImGui();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)m_oApp->GetWindow()->GetWidth(), (float)m_oApp->GetWindow()->GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    scope<GraphicsContext> OpenGLRendererPlugin::CreateGraphicsContext(void* window)
    {
        return fox::new_scope<OpenGLContext>(static_cast<GLFWwindow*>(window));
    }

    ref<Texture2D> OpenGLRendererPlugin::create_texture(uint32_t width, uint32_t height)
    {
        return new_ref<OpenGLTexture>(width, height);
    }

    ref<Texture2D> OpenGLRendererPlugin::create_texture(const std::string& path)
    {
        return new_ref<OpenGLTexture>(path);
    }

    ref<Framebuffer> OpenGLRendererPlugin::create_frame_buffer(uint32_t width, uint32_t height)
    {
        return new_ref<SFMLFrameBuffer>(*this, width, height);
    }

    ref<Camera> OpenGLRendererPlugin::create_camera()
    {
        return new_ref<Camera>();
    }

    ref<EditorCamera> OpenGLRendererPlugin::create_editor_camera()
    {
        return new_ref<SFMLEditorCamera>(*this);
    }

    ref<IndexBuffer> OpenGLRendererPlugin::CreateIndexBuffer(uint32_t* indices, uint32_t size)
    {
        return fox::new_ref<OpenGLIndexBuffer>(indices, size);
    }

    ref<VertexBuffer> OpenGLRendererPlugin::CreateVertexBuffer(float* vertices, uint32_t size)
    {
        return fox::new_ref<OpenGLVertexBuffer>(vertices, size);
    }

    ref<VertexBuffer> OpenGLRendererPlugin::CreateVertexBuffer(uint32_t size)
    {
        return fox::new_ref<OpenGLVertexBuffer>(size);
    }

    ref<VertexArray> OpenGLRendererPlugin::CreateVertexArray()
    {
        return fox::new_ref<OpenGLVertexArray>();
    }

    ref<RendererAPI> OpenGLRendererPlugin::CreateRenderer()
    {
        return new_ref<OpenGLRendererAPI>();
    }

    ref<Shader> OpenGLRendererPlugin::CreateShader(const std::string &path)
    {
        return new_ref<OpenGLShader>(path);
    }

    ref<Shader> OpenGLRendererPlugin::CreateShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc)
    {
        return new_ref<OpenGLShader>(name, vertexSrc, fragSrc);
    }


    void OpenGLRendererPlugin::set_vsync(bool value)
    {
        m_bIsVSync = value;
    }

    bool OpenGLRendererPlugin::is_vsync()
    {
        return m_bIsVSync;
    }

    const std::string &OpenGLRendererPlugin::get_name() const
    {
        return "SFML";
    }

    int OpenGLRendererPlugin::get_version() const
    {
        return 1;
    }
//
//    void OpenGLRendererPlugin::SetRenderTexture(sf::RenderTexture &texture)
//    {
//        m_RenderTexture = &texture;
//    }
//
//    void OpenGLRendererPlugin::SetEditorCamera(sf::View &view)
//    {
//        m_EditorCamera = &view;
//    }
//
//    sf::RenderTexture &OpenGLRendererPlugin::GetRenderTexture()
//    {
//        return *m_RenderTexture;
//    }
//
//    sf::View &OpenGLRendererPlugin::GetEditorCamera()
//    {
//        return *m_EditorCamera;
//    }

    SFMLFrameBuffer::SFMLFrameBuffer(OpenGLRendererPlugin& plugin, uint32_t width, uint32_t height)
    {
//        m_Texture->create(width, height);
//        plugin.SetRenderTexture(*m_Texture);
//        m_Texture->clear(sf::Color::Transparent);
    }

    void SFMLFrameBuffer::Bind()
    {
//        m_Texture->display();
    }

    void SFMLFrameBuffer::Unbind()
    {
//        m_Texture->clear(sf::Color::Blue);
    }

    void SFMLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
//        m_Texture->create(width, height);
    }

    uint32_t SFMLFrameBuffer::GetRendererID() const
    {
        return 0;
    }

    SFMLEditorCamera::SFMLEditorCamera(OpenGLRendererPlugin& plugin) : m_oPlugin(plugin)
    {
//        m_View = plugin.GetRenderTexture().getDefaultView();
//        m_View.setCenter(0, 0);
//        plugin.SetEditorCamera(m_View);
    }

//    SFMLEditorCamera::SFMLEditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
//            : EditorCamera(fov, aspectRatio, nearClip, farClip)
//    {
//    }

    void SFMLEditorCamera::OnZoom(float factor)
    {
//        sf::Vector2i mouse = { static_cast<int>(m_InitialMousePosition.x), static_cast<int>(m_InitialMousePosition.y)};
//        m_View.zoom(factor);
//        m_View.move(((sf::Vector2f(m_oPlugin.GetRenderTexture().mapPixelToCoords(mouse) - m_View.getCenter())).x) * factor,
//      ((sf::Vector2f(m_oPlugin.GetRenderTexture().mapPixelToCoords(mouse) - m_View.getCenter()).y)) * factor);
    }

    void SFMLEditorCamera::OnResize(const Vec2& size)
    {
//        sf::FloatRect visibleArea(m_View.getCenter().x * 2, m_View.getCenter().y * 2, size.x, size.y);
//        m_View = sf::View(visibleArea);
//        m_View.setCenter(m_View.getCenter());
//        m_View.setSize(size.x, size.y);
    }

    void SFMLEditorCamera::OnMove(const Vec2& offset)
    {
//        sf::Vector2f pan = {offset.x, offset.y};
//
//        sf::Transform t;
//        // t.rotate(m_Rotation);
//        pan = t * pan;
//        m_View.move(pan * m_oPlugin.GetApp().get<TimeInfo>()->delta_time);
    }


    void GLFWPlugin::plug(Application &app)
    {
        m_oApp = &app;
        std::cout << "[GLFWPlugin] Init!" << std::endl;
    }

    void GLFWPlugin::unplug(Application &app)
    {
    }

    ref<Window> GLFWPlugin::CreateWindow(const WindowProps& props)
    {
        return fox::new_ref<GlfwWindow>(props);
    }

    const std::string &GLFWPlugin::get_name() const
    {
        return "GLFW";
    }

    int GLFWPlugin::get_version() const
    {
        return 1;
    }
}

extern "C" void RegisterPlugin(fox::PluginManager &pm)
{
    fox::ref<fox::OpenGLRendererPlugin> renderer_plugin = fox::new_ref<fox::OpenGLRendererPlugin>();
    fox::ref<fox::GLFWPlugin> window_plugin = fox::new_ref<fox::GLFWPlugin>();
    pm.AddPlugin(renderer_plugin);
    pm.AddPlugin(window_plugin);
    pm.SetWindowPlugin(window_plugin);
    pm.GetGraphics().AddGraphicsPlugin(std::move(renderer_plugin));
}