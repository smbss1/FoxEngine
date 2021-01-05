
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
#include "UI.h"

namespace Fox
{
    Locator oLocator;
}

using namespace Fox;
namespace fs = std::filesystem;

FoxGUI::UI ui;

Engine::Engine(const std::string& strProjectDir) : m_strProjectDir(strProjectDir), m_strAssetDir(strProjectDir + "/Asset/")
{
    fs::create_directory(m_strAssetDir);
    
    oLocator.RegisterInstance<ResourceManager>();
    oLocator.RegisterInstance(m_oWorld.m_EventManager);

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

void Engine::InitSystems()
{
    // System for when the game start, so start read the scripts
    m_oWorld.system<FoxScript>("ScriptStartSystem")->kind(Fox::Event::Engine::OnStartGame, [](World& w, const Entity& oEntity, FoxScript& oScript)
    {
        std::string strContent;
        if (!File::ReadFile(oScript.m_strFilename, strContent))
            return;
        
        oScript.m_pVm->DefineModule("main");
        Value pGameObject = Fox_DefineInstanceOf(oScript.m_pVm, "fox", "GameObject");
        Fox_SetField(oScript.m_pVm, pGameObject, "m_strName", Fox_String(oScript.m_pVm, "GameObject"));
        Fox_SetField(oScript.m_pVm, pGameObject, "m_iId", Fox_Int(oEntity));
        oScript.m_pVm->DefineVariable("main", "gameObject", pGameObject);
        oScript.m_pVm->Interpret("main", strContent.c_str());

        oScript.OnUpdate = oScript.m_pVm->Function("main", "OnUpdate()");
    });

    // When a script is added on an entity, init the bindings
    m_oWorld.system<FoxScript>("ScriptAddSystem")->kind(Foxecs::System::OnAdd, [](World& w, const Entity& oEntity, FoxScript& oScript)
    {
        oScript.m_pVm = new VM;
        InitBindings(w, oScript.m_pVm);
    });

    // When a script is release, release the memory
    m_oWorld.system<FoxScript>("ScriptRemoveSystem")->kind(Foxecs::System::OnRemove, [](World& w, const Entity& oEntity, FoxScript& oScript)
    {
        oScript.OnUpdate.Release();
        delete oScript.m_pVm;
    });

    // Is the script have an Update function, call it
    m_oWorld.system<FoxScript>("ScriptUpdateSystem")->each([](World& w, const Entity& oEntity, FoxScript& oScript)
    {
        if (oScript.OnUpdate.IsValid())
            oScript.OnUpdate.Call();
    });

    // Draw sprite, if the entity have a sprite component
    m_oWorld.system<Sprite>("SpriteDraw")->kind(Fox::Event::Engine::Gfx::OnRender, [](World& w, const Entity& oEntity, Sprite& oSprite)
    {
        auto window = oLocator.Resolve<sf::RenderWindow>();
        window->draw(oSprite);
    });

    // Update the transform if the entity have Transform component and Sprite Component
    m_oWorld.system<Transform, Sprite>("UpdatePosition")->each([](World& w, const Entity& oEntity, Transform& oTransform, Sprite& oSprite)
    {
        oSprite.setPosition(sf::Vector2f(oTransform.position.x, oTransform.position.y));
        oSprite.setScale(sf::Vector2f(oTransform.scale.x, oTransform.scale.y));
        oSprite.setRotation(atan2(oTransform.rotation.y, oTransform.rotation.x));
    });

    m_oWorld.OnFinishLoaded();
}

void Engine::Run()
{
    sf::RenderWindow* pWIndow = new sf::RenderWindow(sf::VideoMode(800, 600), "Engine");
    oLocator.RegisterInstance(pWIndow);
    sf::Event oWindowEvent;

    Fox::Ecs::Event oRenderEvent(Fox::Event::Engine::Gfx::OnRender);
	oRenderEvent.SetParam(Fox::Event::Engine::Gfx::RENDER_WINDOW, pWIndow);

    Fox::Ecs::Event oUpdateEvent(Foxecs::System::OnUpdate);

    Fox::Ecs::Event oFixedUpdateEvent(Fox::Event::Engine::OnFixedUpdate);

    InitSystems();
    m_oWorld.SendEvent(Fox::Event::Engine::OnStartGame);

    sf::Clock oFrameClock;
    sf::Time oFixedDeltaTime(sf::Time::Zero);
    sf::Time oFixedTimeStep(sf::seconds(1.0f / 45.0f));
    sf::Time oDeltaTime(sf::Time::Zero);

    while (pWIndow->isOpen())
    {
        oFixedDeltaTime += oDeltaTime;
        while (oFixedDeltaTime >= oFixedTimeStep)
        {
            oFixedDeltaTime -= oFixedTimeStep;
            m_oWorld.SendEvent(oFixedUpdateEvent);
            std::cout << "Elapsed: " << oFixedDeltaTime.asSeconds() << std::endl;
        }

        Fox::Ecs::Event event(-1);
        while (pWIndow->pollEvent(oWindowEvent))
        {
            FoxGUI::ProcessEvent(ui, oWindowEvent);
            switch (oWindowEvent.type)
            {
            case sf::Event::Closed:
                pWIndow->close();
                break;

            case sf::Event::Resized:
            {
                sf::FloatRect visibleArea(0, 0, oWindowEvent.size.width, oWindowEvent.size.height);
                pWIndow->setView(sf::View(visibleArea));
                break;
            }

            case sf::Event::MouseButtonPressed:
            {
                event.SetType(Fox::Event::Engine::Input::Mouse::OnMousePressed);
				event.SetParam(Fox::Event::Engine::Input::Mouse::MouseButtonPressed, oWindowEvent.mouseButton);
				m_oWorld.SendEvent(event);
                break;
            }

            case sf::Event::MouseButtonReleased:
            {
                event.SetType(Fox::Event::Engine::Input::Mouse::OnMouseReleased);
				event.SetParam(Fox::Event::Engine::Input::Mouse::MouseButtonReleased, oWindowEvent.mouseButton);
				m_oWorld.SendEvent(event);
                break;
            }

            case sf::Event::MouseMoved:
            {
                event.SetType(Fox::Event::Engine::Input::Mouse::OnMouseMoved);
				event.SetParam(Fox::Event::Engine::Input::Mouse::MouseMoved, oWindowEvent.mouseMove);
				m_oWorld.SendEvent(event);
                break;
            }

            case sf::Event::KeyReleased:
            {
                event.SetType(Fox::Event::Engine::Input::Keyboard::OnKeyRelease);
				event.SetParam(Fox::Event::Engine::Input::Keyboard::KeyReleased, oWindowEvent.key);
				m_oWorld.SendEvent(event);
                break;
            }

            case sf::Event::KeyPressed:
            {
                event.SetType(Fox::Event::Engine::Input::Keyboard::OnKeyPressed);
				event.SetParam(Fox::Event::Engine::Input::Keyboard::KeyPressed, oWindowEvent.key);
				m_oWorld.SendEvent(event);
                break;
            }
            default:
                break;
            }

        }

        // FoxGUI::UIBeginFrame(ui, "", {0, 0}, {300, 150});
        // {
        //     FoxGUI::UIPushRow(ui, {0, 0}, {120, 50});
        //     {
        //         if (FoxGUI::UIButton(ui, "Open Scene"))
        //             std::cout << "Hello!" << std::endl;
        //         if (FoxGUI::UIButton(ui, "Create Entity"))
        //             std::cout << "World!" << std::endl;
        //     }
        //     FoxGUI::UIPopRow(ui);
        // }
        // FoxGUI::UIEndFrame(ui);

        std::cout << oDeltaTime.asSeconds() << std::endl;
	    
        // oUpdateEvent.SetParam(Foxecs::System::DELTA_TIME, oDeltaTime.asSeconds());
		m_oWorld.SendEvent(oUpdateEvent);

        pWIndow->clear();
        FoxGUI::Render(ui, *pWIndow);
        m_oWorld.SendEvent(oRenderEvent);
        pWIndow->display();
        oDeltaTime = oFrameClock.restart();
    }
    m_oWorld.SendEvent(Fox::Event::Engine::OnQuitGame);
    Stop();
}

void Engine::Stop()
{
    // Save::SaveSceneToFile("world.json", m_oWorld);
}