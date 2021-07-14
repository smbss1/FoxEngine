//
// Created by samuel on 29/06/2021.
//

#include "Assets/SubTexture2D.hpp"

namespace fox
{
    SubTexture2D::SubTexture2D(const ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
        : m_pTexture(texture)
    {
        m_TexCoords[0] = { min.x, min.y };
        m_TexCoords[1] = { max.x, min.y };
        m_TexCoords[2] = { max.x, max.y };
        m_TexCoords[3] = { min.x, max.y };
    }

    ref<SubTexture2D> SubTexture2D::CreateFromCoords(const ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 &spriteSize)
    {
        glm::vec2 min = { (coords.x * cellSize.x) / texture->GetWidth(), (coords.y * cellSize.y) / texture->GetHeight() };
        glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight() };
        return new_ref<SubTexture2D>(texture, min, max);
    }
}