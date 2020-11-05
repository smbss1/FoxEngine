
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
    m_oWorld.system<FoxScript>("ScriptStartSystem")->kind(FoxEvent::Engine::OnStartGame, [](World& w, Entity& e)
    {
        FoxScript oScript = w.GetComponent<FoxScript>(e);
        //std::cerr << oScript.m_strFilename << std::endl;
        std::ifstream t(oScript.m_strFilename);
        std::string strContent((std::istreambuf_iterator<char>(t)),
                                std::istreambuf_iterator<char>());
        t.close();
        oScript.m_pVm.Interpret("main", strContent.c_str());

        // oScript.OnUpdate = oScript.m_pVm.Method("main", "OnUpdate");
    });

    // m_oWorld.system<FoxScript>("ScriptUpdateSystem")->each([](World& w, Entity& e)
    // {
    //     FoxScript oScript = w.GetComponent<FoxScript>(e);
    //     oScript.m_pVm.Call(oScript.OnUpdate);
    // });

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