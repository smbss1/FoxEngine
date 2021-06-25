/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Sandbox
*/

#ifndef FOX_SANDBOX_HPP_
#define FOX_SANDBOX_HPP_

#include "FoxEngine.hpp"

class SandboxApp : public fox::Application
{
    public:
        SandboxApp(int argc, char** argv);
        ~SandboxApp() override;
        void init() override;

    protected:
    private:
};

#endif /* !FOX_SANDBOX_HPP_ */
