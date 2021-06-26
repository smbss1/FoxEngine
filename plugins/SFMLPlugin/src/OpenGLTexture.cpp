//
// Created by samuel on 26/06/2021.
//

#include <Renderer.hpp>
#include "stb_image.h"
#include "OpenGLTexture.hpp"

namespace fox
{
    OpenGLTexture::OpenGLTexture(const std::string &path)
        : m_strFilepath(path), m_pLocalBuffer(nullptr), m_iWidth(0)
        , m_iHeight(0), m_BPP(0)
    {
        stbi_set_flip_vertically_on_load(1);
        m_pLocalBuffer = stbi_load(path.c_str(), &m_iWidth, &m_iHeight, &m_BPP, 4);

        GLCall(glGenTextures(1, &m_RendererID));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pLocalBuffer));
        Unbind();

        if (m_pLocalBuffer)
            stbi_image_free(m_pLocalBuffer);
    }

    OpenGLTexture::~OpenGLTexture()
    {
        GLCall(glDeleteTextures(1, &m_RendererID));
    }

    uint32_t OpenGLTexture::GetWidth() const
    {
        return m_iWidth;
    }

    uint32_t OpenGLTexture::GetHeight() const
    {
        return m_iHeight;
    }

    uint32_t OpenGLTexture::GetRendererID() const
    {
        return m_RendererID;
    }

    void OpenGLTexture::SetData(void *data, uint32_t size)
    {

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

    bool OpenGLTexture::operator==(const Texture &other) const
    {
        return false;
    }
}