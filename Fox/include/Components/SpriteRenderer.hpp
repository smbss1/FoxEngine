

#ifndef FOX_SPRITE_RENDERER_HPP_
#define FOX_SPRITE_RENDERER_HPP_

#include <string>

struct SpriteRenderer
{
    SpriteRenderer(const std::string& strPath, const std::string& strId, int d)
        : m_strPath(strPath), m_strId(strId), depth(d)
    {
    }
    std::string m_strPath;
    std::string m_strId;
    int depth = 0;
};

#endif