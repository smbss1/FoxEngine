/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSFML.hpp header
*/

#ifndef LIBSFML_H_
#define LIBSFML_H_

#include <unordered_map>

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
    class OpenGLRendererPlugin : public GraphicPlugin
    {
    public:
        OpenGLRendererPlugin() = default;
        ~OpenGLRendererPlugin();

        void plug(Application& app) override;
        void unplug(Application& app) override;
        const std::string &get_name() const override;
        int get_version() const override;
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
        ref<Shader> CreateShader(const std::string &path) override;
        ref<Shader> CreateShader(const std::string &name, const std::string &vertexSrc, const std::string &fragSrc) override;

        void set_vsync(bool value);
        bool is_vsync();


        Application& GetApp() const { return *m_oApp; }
        // sf::RenderWindow& GetWindow() { return m_oWindow; }

    private:
        // sf::Font m_font;

        bool m_bIsVSync;
        Application* m_oApp = nullptr;
    };

    /**
     * @brief Window Plugin for OpenGL
     * @note I would like to set it in a separate dll library but
     * I can't because using gladLoad && glfwMakeContextCurrent
     * we create two OpenGL Context so can't to separate this two plugins
     */
    class GLFWPlugin : public WindowPlugin
    {
    public:
        GLFWPlugin() = default;
        ~GLFWPlugin() = default;

        void plug(Application& app) override;
        void unplug(Application &app) override;
        ref<Window> CreateWindow(const WindowProps& props) override;

        const std::string &get_name() const override;
        [[nodiscard]] int get_version() const override;

    private:
        Application* m_oApp = nullptr;
    };


    class SFMLFrameBuffer : public Framebuffer
    {
    public:
        SFMLFrameBuffer(OpenGLRendererPlugin& plugin, uint32_t width, uint32_t height);

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
    };

    class SFMLEditorCamera : public EditorCamera
    {
    public:
        explicit SFMLEditorCamera(OpenGLRendererPlugin& plugin);
        // SFMLEditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnZoom(float factor) override;
        void OnResize(const Vec2& size) override;
        void OnMove(const Vec2& offset) override;
    private:

        OpenGLRendererPlugin& m_oPlugin;
    };
}

#endif /* !LIBSFML_H_ */