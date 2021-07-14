
#include <FoxEngine.hpp>
#include <Time.hpp>
#include "Test.hpp"


void Test::OnStart()
{
    fox::info("The game started!!!");

    transform = get_component<fox::TransformComponent>().get();
}

void Test::on_update()
{
    fox::info("The game update!!!");

    transform->position.x += Time::delta_time * 2.0f;
}
