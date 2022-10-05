
#include <FoxEngine.hpp>
#include <Core/EntryPoint.hpp>

#include <FirstTriangle.hpp>
#include <Example2D.hpp>
#include <SpriteSheet.hpp>
//#include <AnimationPlayerState.hpp>

#include "SandboxApp.hpp"

SandboxApp::SandboxApp(fox::ApplicationSpecification spec) : fox::Application(spec)
{
//    PushLayer(new FirstTriangleState);
//    PushLayer(new Example2DState);
    PushLayer(new SpriteSheetLayer);
//    PushLayer(new AnimationPlayerState);
}

SandboxApp::~SandboxApp() { }

fox::Application* fox::CreateApp(fox::ApplicationCommandLineArgs args)
{
    fox::ApplicationSpecification spec;
    spec.Name = "Sandbox";
    return new SandboxApp(spec);
}
