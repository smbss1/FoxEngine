#include "Engine.hpp"
#include "Events.hpp"

void OnStartGame(Fox::Ecs::Event& oEvent)
{
    std::cout << "The game start !!" << std::endl;
}

void OnUpdateGame(Fox::Ecs::Event& oEvent)
{
    std::cout << "The game update !!" << std::endl;
}

void OnRenderGame(Fox::Ecs::Event& oEvent)
{
    std::cout << "The game render !!" << std::endl;
}

int main()
{
    Fox::Engine engine("Project");

    engine.m_oWorld.AddEventListener(Fox::Event::Engine::OnStartGame, OnStartGame);
    engine.m_oWorld.AddEventListener(Foxecs::System::OnUpdate, OnUpdateGame);
    engine.m_oWorld.AddEventListener(Fox::Event::Engine::Gfx::OnRender, OnRenderGame);

    engine.Run();
    return 0;
}