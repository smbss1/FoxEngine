/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSDL2.cpp
*/

#include <cmath>
#include <iostream>
#include "LibSDL2.hpp"
#include "Exceptions/InitException.hpp"
#include "EventName.hpp"
#include "Components/Sprite.hpp"
#include "Components/AsciiSprite.hpp"
#include "Components/Text.hpp"
#include "Components/Transform.hpp"
#include "arial.ttf.h"
#include "IEntity.hpp"

INIT_LIB_API(LibSDL2)

namespace arcade
{
    namespace lib
    {
        LibSDL2::LibSDL2() : AGraphic(), inputEvent(-1)
        {
            m_bQuit = false;
            m_bMouseLeft = false;
            m_bWasMouseLeft = false;
            m_bMouseRight = false;
            m_bWasMouseRight = false;
            m_bMouseMiddle = false;
            m_bWasMouseMiddle = false;
            m_pFont = nullptr;
        }

        LibSDL2::~LibSDL2()
        {
        }

        void LibSDL2::init(IScene &oScene)
        {
            using arcade::component::AsciiSprite;
            using arcade::component::Sprite;
            using arcade::component::Text;
            using arcade::component::Transform;
            std::cout << "[LibSDL2] Init!" << std::endl;

            if (SDL_Init(SDL_INIT_VIDEO) || TTF_Init())
                throw InitException("SDL Initialisation failed.");

            m_pWindow = SDL_CreateWindow("Arcade - LibSDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
            if (!m_pWindow)
                throw InitException("SDL Window creation failed.");
            m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
            if (!m_pRenderer)
                throw InitException("SDL Renderer creation failed.");

            m_pFont = TTF_OpenFontRW(SDL_RWFromMem((void *)ARIALTTF, ARIALTTF_LENGTH), 0, CELL_SIZE);
            if (!m_pFont)
                throw InitException("LibSDL: Can not load arial font");
        }

        void LibSDL2::update(IScene &oScene, float dt)
        {
            using arcade::event::KeyboardEvent;
            using arcade::event::MouseEvent;

            MouseEvent mouse;

            while (SDL_PollEvent(&m_oEvent))
            {
                switch (m_oEvent.type)
                {
                case SDL_QUIT:
                    m_bQuit = true;
                    break;

                case SDL_WINDOWEVENT:
                {
                    if (m_oEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                        SDL_SetWindowSize(m_pWindow, m_oEvent.window.data1, m_oEvent.window.data2);
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    mouse.x = static_cast<int>(m_oEvent.button.x);
                    mouse.y = static_cast<int>(m_oEvent.button.y);
                    mouse.action = MouseEvent::DOWN;

                    if (m_oEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        m_bMouseLeft = true;
                        mouse.button = MouseEvent::MOUSE_PRIMARY;
                        oScene.pushEvent(mouse);
                    }
                    else if (m_oEvent.button.button == SDL_BUTTON_RIGHT)
                    {
                        m_bMouseRight = true;
                        mouse.button = MouseEvent::MOUSE_SECONDARY;
                        oScene.pushEvent(mouse);
                    }
                    else if (m_oEvent.button.button == SDL_BUTTON_MIDDLE)
                    {
                        m_bMouseMiddle = true;
                        mouse.button = MouseEvent::MOUSE_AUXILIARY;
                        oScene.pushEvent(mouse);
                    }
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    mouse.x = static_cast<int>(m_oEvent.button.x);
                    mouse.y = static_cast<int>(m_oEvent.button.y);
                    mouse.action = MouseEvent::RELEASED;

                    if (m_oEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        m_bMouseLeft = false;
                        mouse.button = MouseEvent::MOUSE_PRIMARY;
                        oScene.pushEvent(mouse);
                    }
                    else if (m_oEvent.button.button == SDL_BUTTON_RIGHT)
                    {
                        m_bMouseRight = false;
                        mouse.button = MouseEvent::MOUSE_SECONDARY;
                        oScene.pushEvent(mouse);
                    }
                    else if (m_oEvent.button.button == SDL_BUTTON_MIDDLE)
                    {
                        m_bMouseMiddle = false;
                        mouse.button = MouseEvent::MOUSE_AUXILIARY;
                        oScene.pushEvent(mouse);
                    }
                    break;
                }

                    // case SDL_MOUSEMOTION:
                    // {
                    //     std::cout << m_oEvent.motion.x << std::endl;
                    //     std::cout << m_oEvent.motion.y << std::endl;
                    //     break;
                    // }

                case SDL_KEYUP:
                {
                    KeyboardEvent key;
                    key.key = LibSDL2::SDLKeyEventMap[m_oEvent.key.keysym.sym];
                    key.action = KeyboardEvent::RELEASED;
                    oScene.pushEvent(key);
                    break;
                }

                case SDL_KEYDOWN:
                {
                    KeyboardEvent key;
                    key.key = LibSDL2::SDLKeyEventMap[m_oEvent.key.keysym.sym];
                    if (m_oEvent.key.repeat == 0)
                    {
                        key.action = KeyboardEvent::DOWN;
                        oScene.pushEvent(key);
                    }
                    else
                    {
                        key.action = KeyboardEvent::PRESSED;
                        oScene.pushEvent(key);
                    }
                    break;
                }

                default:
                    break;
                }
                if (mousePressed(mouse))
                    oScene.pushEvent(mouse);
            }

            clear();
            display(oScene);
        }

        void LibSDL2::clear()
        {
            SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
            SDL_RenderClear(m_pRenderer);
        }

        void LibSDL2::display(IScene &oScene)
        {
            using arcade::component::AsciiSprite;
            using arcade::component::Sprite;
            using arcade::component::Text;
            using arcade::component::Transform;

            oScene.forEach([this](arcade::IEntity &e) {
                Sprite *sprite = nullptr;
                AsciiSprite *asciiSprite = nullptr;
                Transform *transform = nullptr;
                if (GetComponent(e, sprite, transform))
                {
                    SDL_Rect rect;
                    rect.x = (int)(transform->position.x * CELL_SIZE);
                    rect.y = (int)(transform->position.y * CELL_SIZE);
                    rect.w = (int)sprite->width;
                    rect.h = (int)sprite->height;
                    if (GetComponent(e, asciiSprite))
                    {
                        rect.w *= (float)(CELL_SIZE * asciiSprite->width) / (float)sprite->width;
                        rect.h *= (float)(CELL_SIZE * asciiSprite->height) / (float)sprite->height;
                    }
                    // SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
                    //     sprite->pixels->data(),
                    //     sprite->width, sprite->height, 32, 4 * sprite->width,
                    //     0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
                    // if (surface)
                    // {
                    //     SDL_Texture *texture = SDL_CreateTextureFromSurface(m_pRenderer, surface);
                    //     if (texture)
                    //     {
                    //         SDL_RenderCopy(m_pRenderer, texture, NULL, &rect);
                    //         SDL_FreeSurface(surface);
                    //     }
                    //     SDL_DestroyTexture(texture);
                    // }
                    for (int i = 0; i < sprite->pixels->size(); ++i) {
                        SDL_SetRenderDrawColor(m_pRenderer, sprite->pixels->at(i).r,
                                                            sprite->pixels->at(i).g,
                                                            sprite->pixels->at(i).b,
                                                            sprite->pixels->at(i).a);
                        int x = (i / 4) % sprite->width;
                        int y = floor((i / 4) / sprite->width);
                        SDL_RenderDrawPoint(m_pRenderer, transform->position.x * CELL_SIZE + x, transform->position.y * CELL_SIZE + y);
                    }
                }
            });
            oScene.forEach([this](arcade::IEntity &e) {
                Text *text = nullptr;
                Transform *transform = nullptr;
                if (GetComponent(e, text, transform))
                {
                    SDL_Color white = {255, 255, 255};
                    SDL_Surface *pSurf = TTF_RenderText_Solid(m_pFont, text->text.c_str(), white);
                    if (pSurf)
                    {
                        SDL_Texture *pTexture = SDL_CreateTextureFromSurface(m_pRenderer, pSurf);
                        if (pTexture)
                        {
                            int texW = 0;
                            int texH = 0;
                            SDL_QueryTexture(pTexture, NULL, NULL, &texW, &texH);
                            SDL_Rect rect;
                            rect.x = (int)(transform->position.x * CELL_SIZE);
                            rect.y = (int)(transform->position.y * CELL_SIZE);
                            rect.w = texW;
                            rect.h = texH;

                            SDL_RenderCopy(m_pRenderer, pTexture, NULL, &rect);
                            SDL_DestroyTexture(pTexture);
                        }
                        SDL_FreeSurface(pSurf);
                    }
                }
            });
            SDL_RenderPresent(m_pRenderer);
        }

        bool LibSDL2::mousePressed(arcade::event::MouseEvent &mouse)
        {
            using arcade::event::MouseEvent;
            bool ret = false;
            if (!m_bWasMouseLeft && m_bMouseLeft)
            {
                mouse.action = MouseEvent::DOWN;
                mouse.button = MouseEvent::MOUSE_PRIMARY;
                ret = true;
            }
            if (!m_bWasMouseRight && m_bMouseRight)
            {
                mouse.action = MouseEvent::DOWN;
                mouse.button = MouseEvent::MOUSE_SECONDARY;
                ret = true;
            }
            if (!m_bWasMouseMiddle && m_bMouseMiddle)
            {
                mouse.action = MouseEvent::DOWN;
                mouse.button = MouseEvent::MOUSE_AUXILIARY;
                ret = true;
            }
            m_bWasMouseLeft = m_bMouseLeft;
            m_bWasMouseRight = m_bMouseRight;
            m_bWasMouseMiddle = m_bMouseMiddle;
            return ret;
        }

        void LibSDL2::end(IScene &oScene)
        {
            SDL_DestroyRenderer(m_pRenderer);
            SDL_DestroyWindow(m_pWindow);
            TTF_CloseFont(m_pFont);
            SDL_Quit();
            TTF_Quit();
        }
    }
}