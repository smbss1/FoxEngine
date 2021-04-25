

#ifndef FOX_SPRITE_RENDERER_HPP_
#define FOX_SPRITE_RENDERER_HPP_

#include <string>

struct SpriteRenderer
{
    SpriteRenderer(const std::string& strPath, const std::string& strId)
        : m_strPath(strPath), m_strId(strId)
    {
    }
    std::string m_strPath;
    std::string m_strId;
};

#endif