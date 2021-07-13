//
// Created by samuel on 26/06/2021.
//

#include <glad/glad.h>
#include <OpenGLRendererAPI.hpp>
#include <Core/Assert.hpp>
#include "stb_image.h"
#include "OpenGLTexture.hpp"

namespace fox
{
    OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height)
            : m_uWidth(width), m_uHeight(height)
    {
        m_InternalFormat = GL_RGBA8;
        m_DataFormat = GL_RGBA;

        GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
        GLCall(glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_uWidth, m_uHeight));

        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT));
    }

    OpenGLTexture::OpenGLTexture(const std::string &path)
        : m_strFilepath(path), m_uWidth(0)
        , m_uHeight(0)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        {
//            HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        FOX_CORE_ASSERT(data, "Failed to load image!");
        m_uWidth = width;
        m_uHeight = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        FOX_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

        GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
        GLCall(glTextureStorage2D(m_RendererID, 1, internalFormat, m_uWidth, m_uHeight));

        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCall(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT));

        GLCall(glTextureSubImage2D(m_RendererID, 0, 0, 0, m_uWidth, m_uHeight, dataFormat, GL_UNSIGNED_BYTE, data));

        stbi_image_free(data);
    }

    OpenGLTexture::~OpenGLTexture()
    {
        GLCall(glDeleteTextures(1, &m_RendererID));
    }

    uint32_t OpenGLTexture::GetWidth() const
    {
        return m_uWidth;
    }

    uint32_t OpenGLTexture::GetHeight() const
    {
        return m_uHeight;
    }

    uint32_t OpenGLTexture::GetRendererID() const
    {
        return m_RendererID;
    }

    void OpenGLTexture::SetData(void *data, uint32_t size)
    {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        FOX_CORE_ASSERT(size == m_uWidth * m_uHeight * bpp, "Data must be entire texture");
        GLCall(glTextureSubImage2D(m_RendererID, 0, 0, 0, m_uWidth, m_uHeight, m_DataFormat, GL_UNSIGNED_BYTE, data));
    }

    void OpenGLTexture::Bind(uint32_t slot) const
    {
        GLCall(glActiveTexture(GL_TEXTURE0 + slot));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    }

    void OpenGLTexture::Unbind() const
    {
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    }

    const std::string& OpenGLTexture::GetId() const
    {
        return m_strFilepath;
    }


    bool OpenGLTexture::operator==(const Texture &other) const
    {
        return m_RendererID == other.GetRendererID();
    }
}