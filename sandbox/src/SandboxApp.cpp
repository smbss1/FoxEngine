
#include <FoxEngine.hpp>

#include <FirstTriangle.hpp>
#include <Example2D.hpp>
#include <SpriteSheet.hpp>

#include "SandboxApp.hpp"
#include "Animator.hpp"

SandboxApp::SandboxApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

SandboxApp::~SandboxApp() { }

void SandboxApp::init()
{
    fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
//    sceneManager.PushState(new FirstTriangleState);
//    sceneManager.PushState(new Example2DState);
    sceneManager.PushState(new SpriteSheetState);
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new SandboxApp(argc, argv);
}