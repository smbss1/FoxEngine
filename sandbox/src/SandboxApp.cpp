
#include <FoxEngine.hpp>
#include <Core/EntryPoint.hpp>

#include "FirstTriangle.hpp"
#include "Example2D.hpp"
#include "SpriteSheet.hpp"
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
    spec.CommandLineArgs = args;
    spec.ScriptSetting.CoreAssemblyPath = "C:/Users/besse/Documents/GitHub/FoxEngine/sandbox/assets/Scripts/Binaries/Fox-ScriptCore.dll";
    spec.ScriptSetting.EnableDebugging = false;
    return new SandboxApp(spec);
}
