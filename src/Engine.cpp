
#include <iostream>
#include <fstream>
#include <streambuf>
#include <signal.h>

#include "foxely.h"
#include "Events.hpp"
#include "FoxEngine.hpp"
#include "Components/FoxScript.hpp"

using namespace Fox;

static bool bIsRunning = true;

void ctrl_c_handler(int sig)
{
    // char  c;

    // signal(sig, SIG_IGN);
    // printf("OUCH, did you hit Ctrl-C?\n"
    //     "Do you really want to quit? [y/n] ");
    // c = getchar();
    // if (c == 'y' || c == 'Y')
    //     exit(0);
    // else
    //     signal(SIGINT, ctrl_c_handler);
    // getchar(); // Get new line character

    bIsRunning = false;
}

Engine::Engine()
{
    struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = ctrl_c_handler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);

//    signal(SIGINT, ctrl_c_handler);
}

void Engine::Start()
{
    m_oWorld.system<FoxScript>("ScriptStartSystem")->kind(FoxEvent::Engine::OnStartGame, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        std::ifstream t(oScript.m_strFilename);
        std::string strContent((std::istreambuf_iterator<char>(t)),
                                std::istreambuf_iterator<char>());
        t.close();
        oScript.m_pVm->Interpret("main", strContent.c_str());

        oScript.OnUpdate = oScript.m_pVm->Function("main", "OnUpdate()");
    });

    m_oWorld.system<FoxScript>("ScriptAddSystem")->kind(Foxecs::System::OnAdd, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        oScript.m_pVm = new VM;
    });

    m_oWorld.system<FoxScript>("ScriptRemoveSystem")->kind(Foxecs::System::OnRemove, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        oScript.OnUpdate.Release();
        delete oScript.m_pVm;
    });

    m_oWorld.system<FoxScript>("ScriptUpdateSystem")->each([](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        if (oScript.OnUpdate.IsValid())
            oScript.OnUpdate.Call();
    });

    Entity e = m_oWorld.CreateEntity();
    m_oWorld.AddComponent<FoxScript>(e, {"Scripts/start.fox"});
    m_oWorld.SendEvent(FoxEvent::Engine::OnStartGame);
}

void Engine::Run()
{
    Start();
    while (bIsRunning)
    {
        m_oWorld.Update();
    }
    Stop();
}

void Engine::Stop()
{
    
}