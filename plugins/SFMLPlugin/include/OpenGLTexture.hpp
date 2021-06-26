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
        explicit OpenGLTexture(const std::string& path);
        ~OpenGLTexture() override;

        [[nodiscard]] uint32_t GetWidth() const override;
        [[nodiscard]] uint32_t GetHeight() const override;
        [[nodiscard]] uint32_t GetRendererID() const override;

        void SetData(void* data, uint32_t size) override;

        void Bind(uint32_t slot = 0) const override;
        void Unbind() const;

        bool operator==(const Texture& other) const override;

    private:
        std::string m_strFilepath;
        unsigned char* m_pLocalBuffer;
        uint32_t m_RendererID{};
        int m_iWidth, m_iHeight, m_BPP;
    };
}


#endif //FOXENGINE_OPENGLTEXTURE_HPP
