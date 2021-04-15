/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** LibSDL2.hpp header
*/

#ifndef LIBSDL2_H_
#define LIBSDL2_H_

#include <memory>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "api.h"
#include "Library/IGraphic.hpp"
#include "Library/AGraphic.hpp"
#include "Event/Event.hpp"
#include "Core/Event.hpp"
#include "Core/World.hpp"
#include "Core/EventManager.hpp"

namespace arcade
{
    namespace lib
    {
        class LibSDL2 : public AGraphic
        {
        public:
            static const library_info_t INFO;
            static std::unordered_map<SDL_Keycode, arcade::event::Key> SDLKeyEventMap;

            LibSDL2();
            ~LibSDL2();
            void init(IScene &oScene) override;
            void update(IScene &oScene, float dt) override;
            void end(IScene &oScene) override;

            void clear();
            void display(IScene &oScene);
            bool mousePressed(arcade::event::MouseEvent &mouse);

        private:
            SDL_Window *m_pWindow;
            SDL_Renderer *m_pRenderer;
            SDL_Event m_oEvent;
            Event inputEvent;

            TTF_Font *m_pFont;

            bool m_bMouseLeft, m_bWasMouseLeft;
            bool m_bMouseRight, m_bWasMouseRight;
            bool m_bMouseMiddle, m_bWasMouseMiddle;
        };

        const library_info_t LibSDL2::INFO = {
            library_info::GRAPHIC,
            "SDL2",
            BUILD_TIME,
            "SDL2: graphical rendering",
        };

        std::unordered_map<SDL_Keycode, arcade::event::Key> LibSDL2::SDLKeyEventMap = {
            {SDLK_ESCAPE, arcade::event::Key::KEY_ESCAPE},
            {SDLK_BACKSPACE, arcade::event::Key::KEY_BACK_SPACE},
            {SDLK_LEFT, arcade::event::Key::KEY_ARROW_LEFT},
            {SDLK_RIGHT, arcade::event::Key::KEY_ARROW_RIGHT},
            {SDLK_UP, arcade::event::Key::KEY_ARROW_UP},
            {SDLK_DOWN, arcade::event::Key::KEY_ARROW_DOWN},
            {SDLK_LSHIFT, arcade::event::Key::KEY_SHIFT_LEFT},
            {SDLK_RSHIFT, arcade::event::Key::KEY_SHIFT_RIGHT},
            {SDLK_LCTRL, arcade::event::Key::KEY_CTRL_LEFT},
            {SDLK_RCTRL, arcade::event::Key::KEY_CTRL_RIGHT},
            {SDLK_LALT, arcade::event::Key::KEY_ALT_LEFT},
            {SDLK_RALT, arcade::event::Key::KEY_ALT_RIGHT},
            {SDLK_PAGEUP, arcade::event::Key::KEY_PAGE_UP},
            {SDLK_PAGEDOWN, arcade::event::Key::KEY_PAGE_DOWN},
            {SDLK_DELETE, arcade::event::Key::KEY_DELETE},
            {SDLK_INSERT, arcade::event::Key::KEY_INSERT},
            {SDLK_END, arcade::event::Key::KEY_END_FILE},
            {SDLK_SPACE, arcade::event::Key::KEY_SPACE},
            {SDLK_F1, arcade::event::Key::KEY_F1},
            {SDLK_F2, arcade::event::Key::KEY_F2},
            {SDLK_F3, arcade::event::Key::KEY_F3},
            {SDLK_F4, arcade::event::Key::KEY_F4},
            {SDLK_F5, arcade::event::Key::KEY_F5},
            {SDLK_F6, arcade::event::Key::KEY_F6},
            {SDLK_F7, arcade::event::Key::KEY_F7},
            {SDLK_F8, arcade::event::Key::KEY_F8},
            {SDLK_F9, arcade::event::Key::KEY_F9},
            {SDLK_F10, arcade::event::Key::KEY_F10},
            {SDLK_F11, arcade::event::Key::KEY_F11},
            {SDLK_F12, arcade::event::Key::KEY_F12},
            {SDLK_a, arcade::event::Key::KEY_A},
            {SDLK_b, arcade::event::Key::KEY_B},
            {SDLK_c, arcade::event::Key::KEY_C},
            {SDLK_d, arcade::event::Key::KEY_D},
            {SDLK_e, arcade::event::Key::KEY_E},
            {SDLK_f, arcade::event::Key::KEY_F},
            {SDLK_g, arcade::event::Key::KEY_G},
            {SDLK_h, arcade::event::Key::KEY_H},
            {SDLK_i, arcade::event::Key::KEY_I},
            {SDLK_j, arcade::event::Key::KEY_J},
            {SDLK_k, arcade::event::Key::KEY_K},
            {SDLK_l, arcade::event::Key::KEY_L},
            {SDLK_m, arcade::event::Key::KEY_M},
            {SDLK_n, arcade::event::Key::KEY_N},
            {SDLK_o, arcade::event::Key::KEY_O},
            {SDLK_p, arcade::event::Key::KEY_P},
            {SDLK_q, arcade::event::Key::KEY_Q},
            {SDLK_r, arcade::event::Key::KEY_R},
            {SDLK_s, arcade::event::Key::KEY_S},
            {SDLK_t, arcade::event::Key::KEY_T},
            {SDLK_u, arcade::event::Key::KEY_U},
            {SDLK_v, arcade::event::Key::KEY_V},
            {SDLK_w, arcade::event::Key::KEY_W},
            {SDLK_x, arcade::event::Key::KEY_X},
            {SDLK_y, arcade::event::Key::KEY_Y},
            {SDLK_z, arcade::event::Key::KEY_Z},
            {SDLK_1, arcade::event::Key::KEY_1},
            {SDLK_2, arcade::event::Key::KEY_2},
            {SDLK_3, arcade::event::Key::KEY_3},
            {SDLK_4, arcade::event::Key::KEY_4},
            {SDLK_5, arcade::event::Key::KEY_5},
            {SDLK_6, arcade::event::Key::KEY_6},
            {SDLK_7, arcade::event::Key::KEY_7},
            {SDLK_8, arcade::event::Key::KEY_8},
            {SDLK_9, arcade::event::Key::KEY_9},
            {SDLK_0, arcade::event::Key::KEY_0},
        };
    }
}

#endif /* !LIBSDL2_H_ */