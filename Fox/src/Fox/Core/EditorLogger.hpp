//
// Created by samuel on 07/10/22.
//

#ifndef FOX_LEXER_EDITORLOGGER_HPP
#define FOX_LEXER_EDITORLOGGER_HPP

void    Clear();
void    AddLog(const char* fmt, ...);
void    DrawLog(const char* title, bool* p_opened = NULL);

#endif //FOX_LEXER_EDITORLOGGER_HPP
