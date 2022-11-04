//
// Created by samuel on 02/11/22.
//

#ifndef FOX_LEXER_EDITORICONS_HPP
#define FOX_LEXER_EDITORICONS_HPP

#include "Renderer/Texture.hpp"

namespace fox
{
    class EditorIcons
    {
    public:
        static void Init();
        static Ref<Texture2D> IconPlay;
        static Ref<Texture2D> IconSimulate;
        static Ref<Texture2D> IconStop;
    };
}

#endif //FOX_LEXER_EDITORICONS_HPP
