/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.cpp
*/

#include <iostream>
#include <Core/Managers/PluginManager.hpp>
#include <Components/EntityName.hpp>
#include <ImGui/imgui-SFML.h>
#include <Core/Managers/StateMachine.hpp>
#include <ImGui/imgui.h>
#include <glad/glad.h>
#include <Time.hpp>
#include <fstream>
#include <OpenGLShader.hpp>
#include <OpenGLTexture.hpp>
#include <ImGui/imgui_impl_opengl3.h>
#include <OpenGLRendererAPI.hpp>
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
    SFMLPlugin::~SFMLPlugin()
    {
//        m_oWindow.close();
    }

    void SFMLPlugin::plug(Application &app)
    {
        m_oApp = &app;
        std::cout << "[SFMLPlugin] Init!" << std::endl;

//        sf::ContextSettings settings;
//
//        settings.majorVersion = 3;
//        settings.minorVersion = 2;
//        settings.attributeFlags = 1;
//
//        settings.depthBits = 24;
//        settings.stencilBits = 8;
//        settings.antialiasingLevel = 2;

//        m_oWindow.create({1280, 720}, "FoxEngine", sf::Style::Default, settings);
        // activate the window
//        m_oWindow.setActive(true);

        // Init glad library
        if (!gladLoadGL())
            exit(84);

//        m_oWindow.setKeyRepeatEnabled(false);
//        m_oWindow.setVerticalSyncEnabled(false);

        fox::info("OpenGL Info:");
        fox::info("   Vendor: %", glGetString(GL_VENDOR));
        fox::info("   Renderer: %", glGetString(GL_RENDERER));
        fox::info("   Version: %", glGetString(GL_VERSION));

        // Stuff for init ImGui in SFML
//        ImGui::SFML::Init(m_oWindow);
//        ImGui_ImplOpenGL3_Init();
//        ImGuiIO& io = ImGui::GetIO();
//        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
//        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
//        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
//        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
//
////        io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Bold.ttf", 18.0f);
////        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Regular.ttf", 18.0f);
//
//        // Setup Dear ImGui style
//        ImGui::StyleColorsDark();
//        //ImGui::StyleColorsClassic();
//
//        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//        ImGuiStyle& style = ImGui::GetStyle();
//        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//        {
//            style.WindowRounding = 0.0f;
//            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//        }


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

//        float positions[] = {
//                -50.0f,  -50.0f, 0.0f, 0.0f, // 0
//                50.0f, -50.0f, 1.0f, 0.0f, // 1
//                50.0f, 50.0f, 1.0f, 1.0f, // 2
//                -50.0f, 50.0f, 0.0f, 1.0f // 3
//        };
//
//        unsigned int indices[] = {
//                0, 1, 2,
//                2, 3, 0
//        };

//        GLCall(glEnable(GL_BLEND));
//        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

//        va = new_scope<OpenGLVertexArray>();
//        vb = new_ref<OpenGLVertexBuffer>(positions, sizeof(positions));
//
//        BufferLayout layout = {
//            {ShaderDataType::Float2, "position"},
//            {ShaderDataType::Float2, "texCoords"}
//        };
//
//        vb->SetLayout(layout);
//        va->AddVertexBuffer(vb);
//
//        ib = new_ref<OpenGLIndexBuffer>(indices, 6);
//        va->SetIndexBuffer(ib);
//
//        proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);
//        view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
//        translationA = glm::vec3(200, 200, 0);
//        translationB = glm::vec3(400, 200, 0);
//
//        shader = new_scope<Shader>("plugins/SFMLPlugin/assets/shaders/Basic.shader");
//        shader->Bind();
//
////        shader->SetUniform("u_Color", 1.0f, 1.0f, 0.0f, 0.0f);
//
//        texture = new_scope<OpenGLTexture>("_fail_.png");
//        texture->Bind();
//
//        shader->SetUniform("u_Texture", 0);
//
//        va->Unbind();
//        shader->Unbind();
//        vb->Unbind();
//        ib->Unbind();
    }

    void SFMLPlugin::unplug(Application &app)
    {
//        ImGui_ImplOpenGL3_Shutdown();
//        ImGui::SFML::Shutdown();
        ib.reset();
        vb.reset();
        shader.reset();
    }

    void SFMLPlugin::poll_event()
    {
//        InputManager& oInputManager = m_oApp->get<InputManager>().value();
//        oInputManager.Reset();
//        while (m_oWindow.pollEvent(m_oEvent))
//        {
//            ImGui::SFML::ProcessEvent(m_oEvent);
//            switch (m_oEvent.type)
//            {
//            case sf::Event::Closed:
//                m_oApp->quit();
//                break;
//
//        //     case sf::Event::Resized:
//        //     {
//        //         sf::FloatRect visibleArea(0, 0, m_oEvent.size.width, m_oEvent.size.height);
//        //         m_oWindow.setView(sf::View(visibleArea));
//        //         break;
//        //     }
//
//             case sf::Event::MouseButtonPressed:
//             {
//                 InputEvent event{};
//                 MouseEvent oMouseEvent {};
//                 MouseButtonEvent oMouseBtnEvent{};
//
//                 // Set the type of the event is Mouse
//                 event.type = InputEventType::MOUSE;
//
//                 // Set that it's a button event
//                 oMouseEvent.type = MouseEventType::BUTTON;
//                 // Set that it's a click
//                 oMouseBtnEvent.state = InputState::PRESSED;
//
//                 // Is Left Click ?
//                 if (m_oEvent.mouseButton.button == sf::Mouse::Button::Left)
//                     oMouseBtnEvent.button = MouseButton::ButtonLeft;
//                 // Is Right Click ?
//                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Right)
//                     oMouseBtnEvent.button = MouseButton::ButtonRight;
//                 // Is Wheel Click ?
//                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Middle)
//                     oMouseBtnEvent.button = MouseButton::ButtonMiddle;
//                 oMouseEvent.button = oMouseBtnEvent;
//                 event.mouse = oMouseEvent;
//                 oInputManager.PushEvent(event);
//                 break;
//             }
//
//             case sf::Event::MouseButtonReleased:
//             {
//                 InputEvent event{};
//                 MouseEvent oMouseEvent {};
//                 MouseButtonEvent oMouseBtnEvent{};
//
//                 // Set the type of the event is Mouse
//                 event.type = InputEventType::MOUSE;
//
//                 // Set that it's a button event
//                 oMouseEvent.type = MouseEventType::BUTTON;
//                 // Set that it's a click
//                 oMouseBtnEvent.state = InputState::RELEASED;
//
//                 // Is Left Click ?
//                 if (m_oEvent.mouseButton.button == sf::Mouse::Button::Left)
//                     oMouseBtnEvent.button = MouseButton::ButtonLeft;
//                     // Is Right Click ?
//                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Right)
//                     oMouseBtnEvent.button = MouseButton::ButtonRight;
//                     // Is Wheel Click ?
//                 else if (m_oEvent.mouseButton.button == sf::Mouse::Button::Middle)
//                     oMouseBtnEvent.button = MouseButton::ButtonMiddle;
//                 oMouseEvent.button = oMouseBtnEvent;
//                 event.mouse = oMouseEvent;
//                 oInputManager.PushEvent(event);
//                 break;
//             }
//
//            case sf::Event::MouseMoved:
//            {
//                InputEvent event{};
//                MouseEvent oMouseEvent {};
//                MouseMoveEvent oMouseMoveEvent{};
//
//                // Set the type of the event is Mouse
//                event.type = InputEventType::MOUSE;
//
//                // Set that it's a move event
//                oMouseEvent.type = MouseEventType::MOVE;
//                // Set the position of the mouse
//                oMouseMoveEvent.x = m_oEvent.mouseMove.x;
//                oMouseMoveEvent.y = m_oEvent.mouseMove.y;
//
//                oMouseEvent.pos = oMouseMoveEvent;
//                event.mouse = oMouseEvent;
//                oInputManager.PushEvent(event);
//                break;
//            }
//
//            case sf::Event::MouseWheelScrolled:
//            {
//                InputEvent event{};
//                MouseEvent oMouseEvent {};
//                MouseScrollEvent oMouseScrollEvent{};
//
//                // Set the type of the event is Mouse
//                event.type = InputEventType::MOUSE;
//
//                // Set that it's a scroll event
//                oMouseEvent.type = MouseEventType::SCROLL;
//                // Set the position of the mouse wheel scroll
//                oMouseScrollEvent.x = m_oEvent.mouseWheelScroll.x;
//                oMouseScrollEvent.y = m_oEvent.mouseWheelScroll.y;
//                oMouseScrollEvent.delta = m_oEvent.mouseWheelScroll.delta;
//
//                oMouseEvent.scroll = oMouseScrollEvent;
//                event.mouse = oMouseEvent;
//                oInputManager.PushEvent(event);
//                break;
//            }
//
//            case sf::Event::KeyReleased:
//            {
//                InputEvent event{};
//                event.type = InputEventType::KEYBOARD;
//                event.keyboard = {SFMLKeyEventMap[m_oEvent.key.code], InputState::RELEASED};
//                oInputManager.PushEvent(event);
//                break;
//            }
//
//            case sf::Event::KeyPressed:
//            {
//                InputEvent event{};
//                event.type = InputEventType::KEYBOARD;
//                event.keyboard = {SFMLKeyEventMap[m_oEvent.key.code], InputState::PRESSED};
//                oInputManager.PushEvent(event);
//                break;
//            }
//
//            default:
//                break;
//            }
//        }

//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
//            puts("key up pressed");
//            _Events.at(_KEY_UP) = true;
//        }
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
//            puts("key down pressed");
//            _Events.at(_KEY_DOWN) = true;
//        }
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
//            puts("key left pressed");
//            _Events.at(_KEY_LEFT ) = true;
//        }
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
//            puts("key right pressed");
//            _Events.at(_KEY_RIGHT) = true;
    }

    void SFMLPlugin::update()
    {
//        ImGui::SFML::Update(m_oWindow, deltaClock.restart());
//
//        renderer->Clear();

//        ImGui_ImplOpenGL3_NewFrame();

        {
//            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
//            glm::mat4 mvp = proj * view * model;
//            shader->Bind();
//            shader->SetUniform("u_MVP", mvp);
//            renderer->Draw(*va, *ib, *shader);
        }

        {
//            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
//            glm::mat4 mvp = proj * view * model;
//            shader->Bind();
//            shader->SetUniform("u_MVP", mvp);
//            renderer->Draw(*va, *ib, *shader);
        }
//         m_oWindow.clear();
//
//        ImGui::SFML::Render(m_oWindow);



//        ImGui::Begin("scsc");
//        {
//            ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 1280.0f);
//            ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 1280.0f);
//        }
//        ImGui::End();


//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



//        m_oApp->get_world().run_phase(ecs::OnStore);
//
//        m_oApp->get<fox::StateMachine>()->UpdateImGui();

//        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//        {
//            GLFWwindow* backup_current_context = m_oWindow.getSystemHandle().;
//            ImGui::UpdatePlatformWindows();
//            ImGui::RenderPlatformWindowsDefault();
//            glfwMakeContextCurrent(backup_current_context);
//        }

//        m_oWindow.display();
    }

    ref<Texture2D> SFMLPlugin::create_texture(uint32_t width, uint32_t height)
    {
        return new_ref<SFMLTexture2D>(width, height);
    }

    ref<Texture2D> SFMLPlugin::create_texture(const std::string& path)
    {
        return new_ref<SFMLTexture2D>(path);
    }

    ref<Framebuffer> SFMLPlugin::create_frame_buffer(uint32_t width, uint32_t height)
    {
        return new_ref<SFMLFrameBuffer>(*this, width, height);
    }

    ref<Camera> SFMLPlugin::create_camera()
    {
        return new_ref<Camera>();
    }

    ref<EditorCamera> SFMLPlugin::create_editor_camera()
    {
        return new_ref<SFMLEditorCamera>(*this);
    }

    ref<IndexBuffer> SFMLPlugin::CreateIndexBuffer(uint32_t* indices, uint32_t size)
    {
        return fox::new_ref<OpenGLIndexBuffer>(indices, size);
    }

    ref<VertexBuffer> SFMLPlugin::CreateVertexBuffer(float* vertices, uint32_t size)
    {
        return fox::new_ref<OpenGLVertexBuffer>(vertices, size);
    }

    ref<VertexArray> SFMLPlugin::CreateVertexArray()
    {
        return fox::new_ref<OpenGLVertexArray>();
    }

    ref<RendererAPI> SFMLPlugin::CreateRenderer()
    {
        return new_ref<OpenGLRendererAPI>();
    }

    ref<Shader> SFMLPlugin::CreateShader(const std::string &filepath)
    {
        return new_ref<OpenGLShader>(filepath);
    }


    void SFMLPlugin::set_vsync(bool value)
    {
        m_bIsVSync = value;
    }

    bool SFMLPlugin::is_vsync()
    {
        return m_bIsVSync;
    }

    const std::string &SFMLPlugin::get_name() const
    {
        return "SFML";
    }

    int SFMLPlugin::get_version() const
    {
        return 1;
    }

    void SFMLPlugin::SetRenderTexture(sf::RenderTexture &texture)
    {
        m_RenderTexture = &texture;
    }

    void SFMLPlugin::SetEditorCamera(sf::View &view)
    {
        m_EditorCamera = &view;
    }

    sf::RenderTexture &SFMLPlugin::GetRenderTexture()
    {
        return *m_RenderTexture;
    }

    sf::View &SFMLPlugin::GetEditorCamera()
    {
        return *m_EditorCamera;
    }

    SFMLTexture2D::SFMLTexture2D(uint32_t width, uint32_t height) : m_Texture(new_scope<sf::Texture>())
    {
        m_Texture->create(width, height);
    }

    SFMLTexture2D::SFMLTexture2D(const std::string &path) : m_Texture(new_scope<sf::Texture>())
    {
        m_Texture->loadFromFile(path);
    }

    SFMLFrameBuffer::SFMLFrameBuffer(SFMLPlugin& plugin, uint32_t width, uint32_t height)
        : m_Texture(new_scope<sf::RenderTexture>())
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
        return m_Texture->getTexture().getNativeHandle();
    }

    SFMLEditorCamera::SFMLEditorCamera(SFMLPlugin& plugin) : m_oPlugin(plugin)
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
        sf::Vector2i mouse = { static_cast<int>(m_InitialMousePosition.x), static_cast<int>(m_InitialMousePosition.y)};
        m_View.zoom(factor);
//        m_View.move(((sf::Vector2f(m_oPlugin.GetRenderTexture().mapPixelToCoords(mouse) - m_View.getCenter())).x) * factor,
//      ((sf::Vector2f(m_oPlugin.GetRenderTexture().mapPixelToCoords(mouse) - m_View.getCenter()).y)) * factor);
    }

    void SFMLEditorCamera::OnResize(const Vec2& size)
    {
//        sf::FloatRect visibleArea(m_View.getCenter().x * 2, m_View.getCenter().y * 2, size.x, size.y);
//        m_View = sf::View(visibleArea);
//        m_View.setCenter(m_View.getCenter());
        m_View.setSize(size.x, size.y);
    }

    void SFMLEditorCamera::OnMove(const Vec2& offset)
    {
        sf::Vector2f pan = {offset.x, offset.y};

        sf::Transform t;
        // t.rotate(m_Rotation);
        pan = t * pan;
        m_View.move(pan * m_oPlugin.GetApp().get<TimeInfo>()->delta_time);
    }
}

extern "C" void RegisterPlugin(fox::PluginManager &pm)
{
    fox::ref<fox::SFMLPlugin> plugin = fox::new_ref<fox::SFMLPlugin>();
    pm.AddPlugin(plugin);
    pm.GetGraphics().AddGraphicsPlugin(std::move(plugin));
}