
#include <iostream>
#include <fstream>
#include <streambuf>
#include <signal.h>
#include <SFML/Graphics.hpp>
#include <cmath>

#include "foxely.h"
#include "Events.hpp"
#include "Engine.hpp"
#include "Components/FoxScript.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "FoxelyBindings.h"

using namespace Fox;

static bool bIsRunning = true;

void ctrl_c_handler(int sig)
{
    signal(sig, SIG_IGN);
    bIsRunning = false;
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
    m_oResourceManager.AddManager("TextureFactory", new TextureManager);
    m_oResourceManager.Add("TextureFactory", "TextureTest", "index.png");

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
        Fox_SetField(oScript.m_pVm, pGameObject, "m_iId", Fox_Int(e));
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

    m_oWorld.system<Sprite>("SpriteDraw")->each([&window](World& w, Entity& e)
    {
        Sprite& oSprite = w.GetComponent<Sprite>(e);
        window.draw(oSprite);
    });

    m_oWorld.system<Transform, Sprite>("UpdatePosition")->each([&window](World& w, Entity& e)
    {
        Sprite& oSprite = w.GetComponent<Sprite>(e);
        Transform& oTransform = w.GetComponent<Transform>(e);

        oSprite.setPosition(sf::Vector2f(oTransform.position.x, oTransform.position.y));
        oSprite.setScale(sf::Vector2f(oTransform.scale.x, oTransform.scale.y));
        oSprite.setRotation(atan2(oTransform.rotation.y, oTransform.rotation.x));
    });

    Entity e = m_oWorld.CreateEntity();
    m_oWorld.AddComponent<FoxScript>(e, { "Scripts/start.fox" });
    m_oWorld.AddComponent<Sprite>(e, Sprite(*m_oResourceManager.Get<sf::Texture>("TextureFactory", "TextureTest")));
    m_oWorld.SendEvent(FoxEvent::Engine::OnStartGame);
}

void Engine::Run()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Lol");
    sf::Event oWindowEvent;
    Start(window);
    while (bIsRunning && window.isOpen())
    {
        while (window.pollEvent(oWindowEvent))
        {
            switch (oWindowEvent.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            
            default:
                break;
            }
        }
        window.clear();
        m_oWorld.Update();
        window.display();
    }
    Stop();
}

void Engine::Stop()
{
    
}