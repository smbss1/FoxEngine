
#pragma once

#include <SFML/Graphics.hpp>

struct Color
{
    int r;
    int g;
    int b;
    int a;
};

struct Sprite
{
    Color color;
    sf::Sprite oSprite;
};