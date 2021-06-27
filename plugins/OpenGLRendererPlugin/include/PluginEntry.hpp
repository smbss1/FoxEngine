/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.hpp header
*/

#ifndef LIBSFML_H_
#define LIBSFML_H_

#include <unordered_map>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>

#include <Core/Application.hpp>
#include <Plugin/IGraphic.hpp>

#include "api.hpp"
#include "Plugin/IPlugin.hpp"
#include "Core/Input/KeyCodes.hpp"
#include "Core/Managers/ResourceManager.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Framebuffer.hpp"
#include "Renderer/EditorCamera.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "OpenGLVertexArray.hpp"

namespace fox
{
    class SFMLPlugin : public GraphicPlugin
    {
    public:
        static std::unordered_map<sf::Keyboard::Key, fox::Key> SFMLKeyEventMap;

        SFMLPlugin() = default;
        ~SFMLPlugin();

        void plug(Application& app) override;
        void unplug(Application& app) override;
        const std::string &get_name() const override;
        int get_version() const override;
        void poll_event() override;
        void update() override;

        scope<GraphicsContext> CreateGraphicsContext(void* window) override;
        ref<Texture2D> create_texture(uint32_t width, uint32_t height) override;
        ref<Texture2D> create_texture(const std::string& path) override;
        ref<Framebuffer> create_frame_buffer(uint32_t width, uint32_t height) override;
        ref<Camera> create_camera() override;
        ref<EditorCamera> create_editor_camera() override;
        ref<IndexBuffer> CreateIndexBuffer(uint32_t* indices, uint32_t size) override;
        ref<VertexBuffer> CreateVertexBuffer(float* vertices, uint32_t size) override;
        ref<VertexArray> CreateVertexArray() override;
        ref<RendererAPI> CreateRenderer() override;
        ref<Shader> CreateShader(const std::string &filepath) override;

        void set_vsync(bool value);
        bool is_vsync();

        void SetRenderTexture(sf::RenderTexture& texture);
        void SetEditorCamera(sf::View& view);

        sf::RenderTexture& GetRenderTexture();
        sf::View& GetEditorCamera();

        Application& GetApp() const { return *m_oApp; }
        // sf::RenderWindow& GetWindow() { return m_oWindow; }

    private:
        sf::RenderWindow m_oWindow;
        sf::Event m_oEvent;

        // sf::Font m_font;

        bool m_bIsVSync;
        Application* m_oApp = nullptr;
        sf::Clock deltaClock;
        sf::RenderTexture* m_RenderTexture = nullptr;
        sf::View* m_EditorCamera = nullptr;

        ref<IndexBuffer> ib;
        ref<VertexBuffer> vb;
        scope<VertexArray> va;
        scope<OpenGLShader> shader;
        scope<Texture2D> texture;
        glm::mat4 proj;
        glm::mat4 view;
        glm::vec3 translationA;
        glm::vec3 translationB;
    };

    class GLFWPlugin : public WindowPlugin
    {
    public:
        GLFWPlugin() = default;
        ~GLFWPlugin() = default;

        void plug(Application& app) override;
        void unplug(Application &app) override;
        ref<Window> CreateWindow(const WindowProps& props = WindowProps()) override;

        const std::string &get_name() const override;
        [[nodiscard]] int get_version() const override;

    private:
        Application* m_oApp = nullptr;
    };



    class SFMLTexture2D : public Texture2D
    {
    public:
        SFMLTexture2D(uint32_t width, uint32_t height);
        explicit SFMLTexture2D(const std::string& path);

        [[nodiscard]] uint32_t GetWidth() const override { return m_Texture->getSize().x; };
        [[nodiscard]] uint32_t GetHeight() const override { return m_Texture->getSize().y; };
        [[nodiscard]] uint32_t GetRendererID() const override { return m_Texture->getNativeHandle(); };

        void SetData(void* data, uint32_t size) override { }

        void Bind(uint32_t slot = 0) const override { }

        bool operator==(const Texture& other) const override { return false; }
    private:
        scope<sf::Texture> m_Texture;
    };

    class SFMLFrameBuffer : public Framebuffer
    {
    public:
        SFMLFrameBuffer(SFMLPlugin& plugin, uint32_t width, uint32_t height);

        void Bind() override;
        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;
        uint32_t GetRendererID() const override;
//        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
//
//        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
//
//        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;
    private:
        scope<sf::RenderTexture> m_Texture;
    };

    class SFMLEditorCamera : public EditorCamera
    {
    public:
        explicit SFMLEditorCamera(SFMLPlugin& plugin);
        // SFMLEditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnZoom(float factor) override;
        void OnResize(const Vec2& size) override;
        void OnMove(const Vec2& offset) override;
    private:

        SFMLPlugin& m_oPlugin;
        sf::View m_View;
    };
}

#endif /* !LIBSFML_H_ */