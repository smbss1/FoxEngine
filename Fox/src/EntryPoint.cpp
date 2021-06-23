/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** EntryPoint.cpp
*/

#include <Logger/Logger.hpp>
#include "FoxEngine.hpp"

extern fox::Application* CreateApp(int argc, char** argv);

int main(int argc, char**argv)
{
    fox::Application *pApp = nullptr;
    try {
        pApp = CreateApp(argc, argv);
        pApp->run();
    } catch (std::exception& e) {
        fox::error("%", e.what());
    }
    fox::info("Application stopped");
    if (pApp)
        delete pApp;
    return 0;
}