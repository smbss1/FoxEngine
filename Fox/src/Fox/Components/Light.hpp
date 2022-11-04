//
// Created by samuel on 27/10/22.
//

#ifndef FOX_LEXER_LIGHT_HPP
#define FOX_LEXER_LIGHT_HPP

struct PointLight
{
    glm::vec2 position;
    glm::vec4 color;
};

struct ShadowCaster
{
    glm::vec2 position;
    glm::vec2 size;
};

#endif //FOX_LEXER_LIGHT_HPP
