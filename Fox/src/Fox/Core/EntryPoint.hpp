/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** EntryPoint.cpp
*/

#pragma once
#ifndef FOX_ENTRY_POINT_HPP_
#define FOX_ENTRY_POINT_HPP_"

extern fox::Application* fox::CreateApp(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    fox::Application *pApp = nullptr;
    try {
        fox::InitializeCore();
        pApp = fox::CreateApp({argc, argv});
        pApp->Run();
    } catch (std::exception& e) {
        FOX_CORE_ERROR("%", e.what());
    }
    if (pApp) {
        FOX_CORE_INFO("Application stopped");
        delete pApp;
        pApp = nullptr;
    }
    std::cout << "Application stopped" << std::endl;
    fox::ShutdownCore();
    return 0;
}

#endif
