
#include <iostream>
#include <fstream>
#include <streambuf>

#include "foxely.h"
#include "Events.hpp"
#include "FoxEngine.hpp"
#include "Components/FoxScript.hpp"

using namespace Fox;

Engine::Engine()
{

}

void Engine::Start()
{
    m_oWorld.system<FoxScript>("ScriptSystem")->kind(FoxEvent::Engine::OnStartGame, [](World& w, Entity& e)
    {
        FoxScript oScript = w.GetComponent<FoxScript>(e);
        std::ifstream t(oScript.strFilename);
        std::string strContent((std::istreambuf_iterator<char>(t)),
                                std::istreambuf_iterator<char>());
        t.close();
        oScript.m_pVm.interpret(strContent.c_str());
    });

    Entity e = m_oWorld.CreateEntity();
    m_oWorld.AddComponent<FoxScript>(e, {"Scripts/start.fox"});
    m_oWorld.SendEvent(FoxEvent::Engine::OnStartGame);
}

bool Engine::Run()
{
    Start();
    m_oWorld.Update();
    Stop();
    return true;
}

void Engine::Stop()
{
    
}