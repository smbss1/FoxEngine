//
// Created by samuel on 29/06/2021.
//

#ifndef FOXENGINE_SUBTEXTURE2D_HPP
#define FOXENGINE_SUBTEXTURE2D_HPP

#include <glm/vec2.hpp>
#include "Texture.hpp"

namespace fox
{
    class SubTexture2D
    {
    public:
        SubTexture2D(const ref<Texture2D> &texture, const glm::vec2& min, const glm::vec2& max);

        [[nodiscard]] const ref<Texture2D>& GetTexture() const { return m_pTexture; }
        [[nodiscard]] const glm::vec2* GetTexCoords() const { return m_TexCoords; }

        static ref<SubTexture2D> CreateFromCoords(const ref<Texture2D> &texture, const glm::vec2 &cellSize, const glm::vec2& coords, const glm::vec2& spriteSize = glm::vec2(1.0f));
    private:
        ref<Texture2D> m_pTexture;
        glm::vec2 m_TexCoords[4];
    };
}

#endif //FOXENGINE_SUBTEXTURE2D_HPP
