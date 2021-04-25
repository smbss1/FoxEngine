/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Core
*/

#ifndef FOX_ENTRYPOINT_HPP_
#define FOX_ENTRYPOINT_HPP_

#ifdef FOX_PLATFORM_LINUX
extern fox::Application* CreateApp(int argc, char** argv);

int main(int argc, char**argv)
{
	fox::Application* pApp = CreateApp(argc, argv);
	pApp->run();
    delete pApp;
    return 0;
}
#endif

#endif /* !FOX_ENTRYPOINT_HPP_ */
