
#include <iostream>
#include <fstream>
#include <streambuf>
#include <signal.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>

#include "foxely.h"
#include "Events.hpp"
#include "Components/FoxScript.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "FoxelyBindings.h"
#include "Paths.hpp"
#include "Save.hpp"
#include "Engine.hpp"

namespace Fox
{
    Locator oLocator;
}

using namespace Fox;
namespace fs = std::filesystem;

static bool bIsRunning = true;

void ctrl_c_handler(int sig)
{
    signal(sig, SIG_IGN);
    bIsRunning = false;
}

Engine::Engine(const std::string& strProjectDir) : m_strProjectDir(strProjectDir), m_strAssetDir(strProjectDir + "/Asset/")
{
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    fs::create_directory(m_strAssetDir);
    
    oLocator.RegisterInstance<ResourceManager>();
    oLocator.RegisterInstance(m_oWorld.m_EventManager);
    oLocator.RegisterInstance<Simple::Signal<void ()>>();

    std::shared_ptr<ResourceManager> pResourceManager = oLocator.Resolve<ResourceManager>();
    if (pResourceManager == nullptr)
    {
        std::cerr << "ResourceManager is null" << std::endl;
        return;
    }

    pResourceManager->AddManager("TextureFactory", new TextureManager);
    pResourceManager->Add("TextureFactory", "_fail_", "_fail_.png");

    Path::Paths vFiles = Path::GetAllFilesInDir(m_strAssetDir, ".png", true);
    for (auto& oFile : vFiles)
    {
        std::string strAssetName = Path::GetBaseFilename(oFile);
        pResourceManager->Add("TextureFactory", strAssetName, m_strAssetDir + oFile);
    }
    Save::LoadSceneFromFile("world.json", m_oWorld);
}

void Engine::Start(sf::RenderWindow& window)
{
    // System for when the game start, so start read the scripts
    m_oWorld.system<FoxScript>("ScriptStartSystem")->kind(FoxEvent::Engine::OnStartGame, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        std::string strContent;
        if (!File::ReadFile(oScript.m_strFilename, strContent))
            return;
        
        oScript.m_pVm->DefineModule("main");
        Value pGameObject = Fox_DefineInstanceOf(oScript.m_pVm, "fox", "GameObject");
        Fox_SetField(oScript.m_pVm, pGameObject, "m_strName", Fox_String(oScript.m_pVm, "GameObject"));
        Fox_SetField(oScript.m_pVm, pGameObject, "m_iId", Fox_Int(e));
        oScript.m_pVm->DefineVariable("main", "gameObject", pGameObject);
        oScript.m_pVm->Interpret("main", strContent.c_str());

        oScript.OnUpdate = oScript.m_pVm->Function("main", "OnUpdate()");
    });

    // When a script is added on an entity, init the bindings
    m_oWorld.system<FoxScript>("ScriptAddSystem")->kind(Foxecs::System::OnAdd, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        oScript.m_pVm = new VM;
        InitBindings(w, oScript.m_pVm);
    });

    // When a script is release, release the memory
    m_oWorld.system<FoxScript>("ScriptRemoveSystem")->kind(Foxecs::System::OnRemove, [](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);
        oScript.OnUpdate.Release();
        delete oScript.m_pVm;
    });

    // Is the script have an Update function, call it
    m_oWorld.system<FoxScript>("ScriptUpdateSystem")->each([](World& w, Entity& e)
    {
        FoxScript& oScript = w.GetComponent<FoxScript>(e);

        if (oScript.OnUpdate.IsValid())
            oScript.OnUpdate.Call();
    });

    // Draw sprite, if the entity have a sprite component
    m_oWorld.system<Sprite>("SpriteDraw")->each([&window](World& w, Entity& e)
    {
		std::cout << "Draw" << std::endl;
        Sprite& oSprite = w.GetComponent<Sprite>(e);
        window.draw(oSprite);
    });

    // Update the transform if the entity have Transform component and Sprite Component
    m_oWorld.system<Transform, Sprite>("UpdatePosition")->each([&window](World& w, Entity& e)
    {
        Sprite& oSprite = w.GetComponent<Sprite>(e);
        Transform& oTransform = w.GetComponent<Transform>(e);

        oSprite.setPosition(sf::Vector2f(oTransform.position.x, oTransform.position.y));
        oSprite.setScale(sf::Vector2f(oTransform.scale.x, oTransform.scale.y));
        oSprite.setRotation(atan2(oTransform.rotation.y, oTransform.rotation.x));
    });

    m_oWorld.OnFinishLoaded();

    // Entity e = m_oWorld.CreateEntity();
    // m_oWorld.AddComponent<FoxScript>(e, FoxScript("Scripts/start.fox"));
    // m_oWorld.AddComponent<Sprite>(e, Sprite("index"));
    // m_oWorld.AddComponent<Transform>(e, Transform({500, 500}));
    m_oWorld.SendEvent(FoxEvent::Engine::OnStartGame);
}

void Engine::Run()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Lol");
    sf::Event oWindowEvent;

    Event oRenderEvent(FoxEvent::Engine::Window::OnRender);
	oRenderEvent.SetParam(FoxEvent::Engine::Window::RENDER_WINDOW, &window);

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
            
            case sf::Event::KeyReleased:
            case sf::Event::KeyPressed:
            {
            	Event event(FoxEvent::Engine::Window::OnInput);
				event.SetParam(FoxEvent::Engine::Window::INPUT, oWindowEvent.key);
				m_oWorld.SendEvent(event);
                break;
            }
            
            default:
                break;
            }
        }

        window.clear();
        // m_oWorld.SendEvent(oRenderEvent);
        m_oWorld.Update();
        window.display();
    }
    Stop();
}

void Engine::Stop()
{
    Save::SaveSceneToFile("world.json", m_oWorld);
}