// /*
// ** EPITECH PROJECT, 2021
// ** Arcade
// ** File description:
// ** LibSFML.cpp
// */

// #include <iostream>
// #include <Core/Managers/PluginManager.hpp>
// #include <Components/EntityName.hpp>
// #include <Core/Managers/StateMachine.hpp>
// #include <glad/glad.h>
// #include <OpenGLShader.hpp>
// #include <OpenGLTexture.hpp>
// #include <OpenGLRendererAPI.hpp>
// #include <OpenGLContext.hpp>
// #include <OpenGLFrameBuffer.hpp>
// #include "GlfwWindow.hpp"
// #include "Core/Input/InputManager.hpp"
// #include "TextureManager.hpp"
// #include "SpriteManager.hpp"
// #include "PluginEntry.hpp"
// #include "VertexBuffer.hpp"
// #include "IndexBuffer.hpp"

// namespace fox
// {
//     OpenGLRendererPlugin::~OpenGLRendererPlugin()
//     {
//     }

//     void OpenGLRendererPlugin::plug(Application &app)
//     {
//         // Add the ImGui state of GUI
//         m_pImGuiState = new ImGuiState;
//         app.get<StateMachine>()->PushOverlay(m_pImGuiState);

//         m_oApp = &app;
//         std::cout << "[OpenGLRendererPlugin] Init!" << std::endl;


// //        ResourceManager& resource_manager = app.get<ResourceManager>().value();
// //        resource_manager.AddManager<sf::Texture>(new TextureManager(resource_manager));
// //        resource_manager.AddManager<sf::Sprite>(new SpriteManager(resource_manager));
// //
// //        app.get_world().system<SpriteRenderer>("InitSpriteRenderer")
// //            .kind(ecs::OnAdd).each([&](Entity& e, SpriteRenderer& renderer)
// //               {
// //                   resource_manager.AddAsset<sf::Texture>(renderer.m_strId, renderer.m_strPath);
// //                   resource_manager.AddAsset<sf::Sprite>(renderer.m_strId, renderer.m_strPath);
// //               });
// //
// //        app.get_world().system<Transform, SpriteRenderer>("DrawSpriteRenderer")
// //            .order_by([](Transform& t1, SpriteRenderer& r1, Transform& t2, SpriteRenderer& r2)
// //              {
// //                  return r1.depth < r2.depth;
// //              })
// //            .kind(ecs::OnStore).each([&](Entity& e, Transform& transform, SpriteRenderer& renderer)
// //                 {
// //                     auto* sprite = resource_manager.GetAsset<sf::Sprite>(renderer.m_strId);
// //                     sprite->setPosition(transform.position.x, transform.position.y);
// //                     m_RenderTexture->setView(*m_EditorCamera);
// //                     if (m_RenderTexture)
// //                         m_RenderTexture->draw(*sprite);
// //                     else
// //                        m_oWindow.draw(*sprite);
// //                 });
//     }

//     void OpenGLRendererPlugin::unplug(Application &app)
//     {
//     }

//     void OpenGLRendererPlugin::update()
//     {
//         m_pImGuiState->Begin();
//         if (!m_oApp->IsWindowMinized())
//             m_oApp->get<fox::StateMachine>()->UpdateImGui();
//         m_pImGuiState->End();
//     }

//     scope<GraphicsContext> OpenGLRendererPlugin::CreateGraphicsContext(void* window)
//     {
//         return fox::new_scope<OpenGLContext>(static_cast<GLFWwindow*>(window));
//     }

//     ref<Texture2D> OpenGLRendererPlugin::create_texture(uint32_t width, uint32_t height)
//     {
//         return new_ref<OpenGLTexture>(width, height);
//     }

//     ref<Texture2D> OpenGLRendererPlugin::create_texture(const std::string& path)
//     {
//         return new_ref<OpenGLTexture>(path);
//     }

//     ref<Framebuffer> OpenGLRendererPlugin::CreateFramebuffer(const FramebufferSpecification &spec)
//     {
//         return new_ref<OpenGLFrameBuffer>(spec);
//     }

//     ref<Camera> OpenGLRendererPlugin::create_camera()
//     {
//         return new_ref<Camera>();
//     }

//     ref<IndexBuffer> OpenGLRendererPlugin::CreateIndexBuffer(uint32_t* indices, uint32_t size)
//     {
//         return fox::new_ref<OpenGLIndexBuffer>(indices, size);
//     }

//     ref<VertexBuffer> OpenGLRendererPlugin::CreateVertexBuffer(float* vertices, uint32_t size)
//     {
//         return fox::new_ref<OpenGLVertexBuffer>(vertices, size);
//     }

//     ref<VertexBuffer> OpenGLRendererPlugin::CreateVertexBuffer(uint32_t size)
//     {
//         return fox::new_ref<OpenGLVertexBuffer>(size);
//     }

//     ref<VertexArray> OpenGLRendererPlugin::CreateVertexArray()
//     {
//         return fox::new_ref<OpenGLVertexArray>();
//     }

//     ref<RendererAPI> OpenGLRendererPlugin::CreateRenderer()
//     {
//         return new_ref<OpenGLRendererAPI>();
//     }

//     ref<Shader> OpenGLRendererPlugin::CreateShader(const std::string &path)
//     {
//         return new_ref<OpenGLShader>(path);
//     }

//     ref<Shader> OpenGLRendererPlugin::CreateShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc)
//     {
//         return new_ref<OpenGLShader>(name, vertexSrc, fragSrc);
//     }

//     const std::string &OpenGLRendererPlugin::get_name() const
//     {
//         return "SFML";
//     }

//     int OpenGLRendererPlugin::get_version() const
//     {
//         return 1;
//     }

//     void GLFWPlugin::plug(Application &app)
//     {
//         m_oApp = &app;
//         std::cout << "[GLFWPlugin] Init!" << std::endl;
//     }

//     void GLFWPlugin::unplug(Application &app)
//     {
//     }

//     // Glfw Window
//     ref<Window> GLFWPlugin::CreateWindow(const WindowProps& props)
//     {
//         return fox::new_ref<GlfwWindow>(props);
//     }

//     const std::string &GLFWPlugin::get_name() const
//     {
//         return "GLFW";
//     }

//     int GLFWPlugin::get_version() const
//     {
//         return 1;
//     }
// }

// extern "C" void RegisterPlugin(fox::PluginManager &pm)
// {
//     fox::ref<fox::OpenGLRendererPlugin> renderer_plugin = fox::new_ref<fox::OpenGLRendererPlugin>();
//     fox::ref<fox::GLFWPlugin> window_plugin = fox::new_ref<fox::GLFWPlugin>();
//     pm.AddPlugin(renderer_plugin);
//     pm.AddPlugin(window_plugin);
//     pm.SetWindowPlugin(window_plugin);
//     pm.GetGraphics().AddGraphicsPlugin(std::move(renderer_plugin));
// }