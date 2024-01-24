//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_OPENGLTEXTURE_HPP
#define FOXENGINE_OPENGLTEXTURE_HPP

#include <glad/glad.h>
#include "Renderer/Texture.hpp"

namespace fox
{
    class OpenGLTexture : public Texture2D
    {
    public:
        OpenGLTexture(const TextureSpecification& specification, Buffer data = Buffer());
        OpenGLTexture(uint32_t width, uint32_t height);
        explicit OpenGLTexture(const std::string& path);
        ~OpenGLTexture() override;

        [[nodiscard]] uint32_t GetWidth() const override;
        [[nodiscard]] uint32_t GetHeight() const override;
        [[nodiscard]] uint32_t GetRendererID() const override;

        virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

        void SetData(Buffer data) override;

        void Bind(uint32_t slot = 0) const override;
        void Unbind() const override;

        const std::string& GetId() const override;
        bool IsLoaded() const override { return m_IsLoaded; }

        bool operator==(const Texture& other) const override;

    private:
        TextureSpecification m_Specification;

        std::string m_strFilepath;
        uint32_t m_RendererID{};
        uint32_t m_Width, m_Height;
        GLenum m_InternalFormat, m_DataFormat;
        bool m_IsLoaded = false;
    };
}


#endif //FOXENGINE_OPENGLTEXTURE_HPP
