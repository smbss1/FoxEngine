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

    std::unordered_map<sf::Keyboard::Key, fox::Key> SFMLPlugin::SFMLKeyEventMap = {
        {sf::Keyboard::A, fox::Key::A},
        {sf::Keyboard::B, fox::Key::B},
        {sf::Keyboard::C, fox::Key::C},
        {sf::Keyboard::D, fox::Key::D},
        {sf::Keyboard::E, fox::Key::E},
        {sf::Keyboard::F, fox::Key::F},
        {sf::Keyboard::G, fox::Key::G},
        {sf::Keyboard::H, fox::Key::H},
        {sf::Keyboard::I, fox::Key::I},
        {sf::Keyboard::J, fox::Key::J},
        {sf::Keyboard::K, fox::Key::K},
        {sf::Keyboard::L, fox::Key::L},
        {sf::Keyboard::M, fox::Key::M},
        {sf::Keyboard::N, fox::Key::N},
        {sf::Keyboard::O, fox::Key::O},
        {sf::Keyboard::P, fox::Key::P},
        {sf::Keyboard::Q, fox::Key::Q},
        {sf::Keyboard::R, fox::Key::R},
        {sf::Keyboard::S, fox::Key::S},
        {sf::Keyboard::T, fox::Key::T},
        {sf::Keyboard::U, fox::Key::U},
        {sf::Keyboard::V, fox::Key::V},
        {sf::Keyboard::W, fox::Key::W},
        {sf::Keyboard::X, fox::Key::X},
        {sf::Keyboard::Y, fox::Key::Y},
        {sf::Keyboard::Z, fox::Key::Z},
        {sf::Keyboard::Num0, fox::Key::D0},
        {sf::Keyboard::Num1, fox::Key::D1},
        {sf::Keyboard::Num2, fox::Key::D2},
        {sf::Keyboard::Num3, fox::Key::D3},
        {sf::Keyboard::Num4, fox::Key::D4},
        {sf::Keyboard::Num5, fox::Key::D5},
        {sf::Keyboard::Num6, fox::Key::D6},
        {sf::Keyboard::Num7, fox::Key::D7},
        {sf::Keyboard::Num8, fox::Key::D8},
        {sf::Keyboard::Num9, fox::Key::D9},
        {sf::Keyboard::Numpad0, fox::Key::KP0},
        {sf::Keyboard::Numpad1, fox::Key::KP1},
        {sf::Keyboard::Numpad2, fox::Key::KP2},
        {sf::Keyboard::Numpad3, fox::Key::KP3},
        {sf::Keyboard::Numpad4, fox::Key::KP4},
        {sf::Keyboard::Numpad5, fox::Key::KP5},
        {sf::Keyboard::Numpad6, fox::Key::KP6},
        {sf::Keyboard::Numpad7, fox::Key::KP7},
        {sf::Keyboard::Numpad8, fox::Key::KP8},
        {sf::Keyboard::Numpad9, fox::Key::KP9},
        {sf::Keyboard::F1, fox::Key::F1},
        {sf::Keyboard::F2, fox::Key::F2},
        {sf::Keyboard::F3, fox::Key::F3},
        {sf::Keyboard::F4, fox::Key::F4},
        {sf::Keyboard::F5, fox::Key::F5},
        {sf::Keyboard::F6, fox::Key::F6},
        {sf::Keyboard::F7, fox::Key::F7},
        {sf::Keyboard::F8, fox::Key::F8},
        {sf::Keyboard::F9, fox::Key::F9},
        {sf::Keyboard::F10, fox::Key::F10},
        {sf::Keyboard::F11, fox::Key::F11},
        {sf::Keyboard::F12, fox::Key::F12},
        {sf::Keyboard::F13, fox::Key::F13},
        {sf::Keyboard::F14, fox::Key::F14},
        {sf::Keyboard::F15, fox::Key::F15},

        {sf::Keyboard::Escape, fox::Key::Escape},
        {sf::Keyboard::Backspace, fox::Key::Backspace},
        {sf::Keyboard::Space, fox::Key::Space},
        {sf::Keyboard::Left, fox::Key::Left},
        {sf::Keyboard::Right, fox::Key::Right},
        {sf::Keyboard::Up, fox::Key::Up},
        {sf::Keyboard::Down, fox::Key::Down},
        {sf::Keyboard::LShift, fox::Key::LeftShift},
        {sf::Keyboard::RShift, fox::Key::RightShift},
        {sf::Keyboard::LControl, fox::Key::LeftControl},
        {sf::Keyboard::RControl, fox::Key::RightControl},
        {sf::Keyboard::LAlt, fox::Key::LeftAlt},
        {sf::Keyboard::RAlt, fox::Key::RightAlt},
        {sf::Keyboard::PageUp, fox::Key::PageUp},
        {sf::Keyboard::PageDown, fox::Key::PageDown},
        {sf::Keyboard::Delete, fox::Key::Delete},
        {sf::Keyboard::Insert, fox::Key::Insert},
        {sf::Keyboard::End, fox::Key::End},
        {sf::Keyboard::Semicolon, fox::Key::Semicolon},
        {sf::Keyboard::Equal, fox::Key::Equal},
        {sf::Keyboard::Comma, fox::Key::Comma},
        {sf::Keyboard::Subtract, fox::Key::Minus},
        {sf::Keyboard::Slash, fox::Key::Slash},
        {sf::Keyboard::Period, fox::Key::Period},
        {sf::Keyboard::LBracket, fox::Key::LeftBracket},
        {sf::Keyboard::RBracket, fox::Key::RightBracket},
        {sf::Keyboard::Tab, fox::Key::Tab},
        {sf::Keyboard::Home, fox::Key::Home},
        {sf::Keyboard::Multiply, fox::Key::KPMultiply},
        {sf::Keyboard::Pause, fox::Key::Pause},
        {sf::Keyboard::Add, fox::Key::KPAdd},
        {sf::Keyboard::Divide, fox::Key::KPDivide},
        {sf::Keyboard::RSystem, fox::Key::RightSuper},
        {sf::Keyboard::LSystem, fox::Key::LeftSuper},
    };
}

#endif /* !LIBSFML_H_ */