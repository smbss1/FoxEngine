//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_OPENGLTEXTURE_HPP
#define FOXENGINE_OPENGLTEXTURE_HPP

#include "Renderer/Texture.hpp"
namespace fox
{
    class OpenGLTexture : public Texture2D
    {
    public:
        OpenGLTexture(uint32_t width, uint32_t height);
        explicit OpenGLTexture(const std::string& path);
        ~OpenGLTexture() override;

        [[nodiscard]] uint32_t GetWidth() const override;
        [[nodiscard]] uint32_t GetHeight() const override;
        [[nodiscard]] uint32_t GetRendererID() const override;

        void SetData(void* data, uint32_t size) override;

        void Bind(uint32_t slot = 0) const override;
        void Unbind() const;

        const std::string& GetId() const override;


        bool operator==(const Texture& other) const override;

    private:
        std::string m_strFilepath;
        uint32_t m_RendererID{};
        uint32_t m_uWidth, m_uHeight;
        GLenum m_InternalFormat, m_DataFormat;
    };
}


#endif //FOXENGINE_OPENGLTEXTURE_HPP
