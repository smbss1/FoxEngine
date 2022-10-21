
#include <FoxEngine.hpp>
#include "Save for later/Time.hpp"
#include "Test.hpp"


void Test::OnStart()
{
    FOX_INFO("The game started!!!");

    transform = &get_component<fox::TransformComponent>().value();
}

void Test::on_update()
{
    FOX_INFO("The game update!!!");

    transform->position.x += Time::delta_time * 2.0f;
}
