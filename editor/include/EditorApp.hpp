/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Sandbox
*/

#ifndef FOX_EDITOR_APP_HPP_
#define FOX_EDITOR_APP_HPP_

#include "FoxEngine.hpp"

class EditorApp : public fox::Application
{
    public:
    EditorApp(int argc, char** argv);
        ~EditorApp() override;
        void init() override;

    protected:
    private:
};

#endif /* !FOX_EDITOR_APP_HPP_ */
