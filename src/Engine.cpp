
#include <iostream>
#include <fstream>
#include <streambuf>
#include <signal.h>
#include <SFML/Graphics.hpp>

#include "foxely.h"
#include "Events.hpp"
#include "FoxEngine.hpp"
#include "Components/FoxScript.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "FoxelyBindings.h"

using namespace Fox;

static bool bIsRunning = true;

void ctrl_c_handler(int sig)
{
    char  c;

    signal(sig, SIG_IGN);
    printf("OUCH, did you hit Ctrl-C?\n"
        "Do you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
        bIsRunning = false;
    else
        signal(SIGINT, ctrl_c_handler);
    getchar(); // Get new line character

}

Engine::Engine()
{
    struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = ctrl_c_handler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);
}

void Engine::Start(sf::RenderWindow& window)
{
    m_oWorld.system<FoxScript>("ScriptStartSystem")->kind(FoxEvent::Engine::OnStartGame, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        std::ifstream t(oScript.m_strFilename);
        std::string strContent((std::istreambuf_iterator<char>(t)),
                                std::istreambuf_iterator<char>());
        t.close();
        
        oScript.m_pVm->DefineModule("main");
        Value pGameObject = Fox_DefineInstanceOf(oScript.m_pVm, "fox", "GameObject");
        Fox_SetField(oScript.m_pVm, pGameObject, "m_strName", Fox_String(oScript.m_pVm, "GameObject"));
        Fox_SetField(oScript.m_pVm, pGameObject, "m_iId", Fox_IntegerToValue(e));
        oScript.m_pVm->DefineVariable("main", "gameObject", pGameObject);
        oScript.m_pVm->Interpret("main", strContent.c_str());

        oScript.OnUpdate = oScript.m_pVm->Function("main", "OnUpdate()");
    });

    m_oWorld.system<FoxScript>("ScriptAddSystem")->kind(Foxecs::System::OnAdd, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        oScript.m_pVm = new VM;
        InitBindings(w, oScript.m_pVm);
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

    m_oWorld.system<Sprite>("SpriteUpdate")->each([&window](World& w, Entity& e)
    {
        Sprite& oSprite = w.GetComponent<Sprite>(e);
        window.draw(oSprite.oSprite);

        // if (oScript.OnUpdate.IsValid())
            // oScript.OnUpdate.Call();
    });

    Entity e = m_oWorld.CreateEntity();
    m_oWorld.AddComponent<FoxScript>(e, { "Scripts/start.fox" });
    m_oWorld.AddComponent<Sprite>(e, { });
    Sprite& s = m_oWorld.GetComponent<Sprite>(e);
    sf::Texture t;
    t.loadFromFile("index.png");
    s.oSprite.setTexture(t);
    m_oWorld.SendEvent(FoxEvent::Engine::OnStartGame);
}

void Engine::Run()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Lol");
    Start(window);
    while (bIsRunning)
    {
        window.clear();
        m_oWorld.Update();
        window.display();
    }
    Stop();
}

void Engine::Stop()
{
    
}