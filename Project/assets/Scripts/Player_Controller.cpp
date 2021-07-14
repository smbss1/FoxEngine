
#include <FoxEngine.hpp>
#include <Core/Input/Input.hpp>
#include <Utils/Time.hpp>
#include "Player_Controller.hpp"


void Player_Controller::OnStart()
{
    transform = &*get_component<fox::TransformComponent>();
}

void Player_Controller::on_update()
{
    if (fox::Input::IsKeyPressed(fox::Key::W))
        transform->position.y += m_fSpeed * Time::delta_time;

    if (fox::Input::IsKeyPressed(fox::Key::S))
        transform->position.y -= m_fSpeed * Time::delta_time;

    if (fox::Input::IsKeyPressed(fox::Key::A))
        transform->position.x -= m_fSpeed * Time::delta_time;

    if (fox::Input::IsKeyPressed(fox::Key::D))
        transform->position.x += m_fSpeed * Time::delta_time;
}
