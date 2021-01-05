
#pragma once

#include "Core/Types.hpp"

namespace Fox::Event::Engine
{
	const EventId OnStartGame = "Fox::Event::Engine::OnStartGame"_hash;
	const EventId OnFixedUpdate = "Fox::Event::Engine::OnFixedUpdate"_hash;
	const EventId OnQuitGame = "Fox::Event::Engine::OnQuitGame"_hash;
}

namespace Fox::Event::Engine::Gfx
{
	const EventId OnRender = "Fox::Event::Engine::Gfx::OnRender"_hash;

	const ParamId RENDER_WINDOW = "Fox::Event::Engine::Gfx::RENDER_WINDOW"_hash;
}

namespace Fox::Event::Engine::Input::Keyboard
{
	const EventId OnKey = "Fox::Event::Engine::Input::Keyboard::OnKey"_hash;
	const EventId OnKeyPressed = "Fox::Event::Engine::Input::Keyboard::OnKeyPressed"_hash;
	const EventId OnKeyRelease = "Fox::Event::Engine::Input::Keyboard::OnKeyRelease"_hash;

	const ParamId Key = "Fox::Event::Engine::Input::Keyboard::Key"_hash;
	const ParamId KeyPressed = "Fox::Event::Engine::Input::Keyboard::KeyPressed"_hash;
	const ParamId KeyReleased = "Fox::Event::Engine::Input::Keyboard::KeyReleased"_hash;
}

namespace Fox::Event::Engine::Input::Mouse
{
	const EventId OnMouse = "Fox::Event::Engine::Input::Mouse::OnMouse"_hash;
	const EventId OnMousePressed = "Fox::Event::Engine::Input::Mouse::OnMousePressed"_hash;
	const EventId OnMouseReleased = "Fox::Event::Engine::Input::Mouse::OnMouseReleased"_hash;
	const EventId OnMouseMoved = "Fox::Event::Engine::Input::Mouse::OnMouseMoved"_hash;

	const ParamId MouseButton = "Fox::Event::Engine::Input::Mouse::MouseButton"_hash;
	const ParamId MouseButtonPressed = "Fox::Event::Engine::Input::Mouse::MouseButtonPressed"_hash;
	const ParamId MouseButtonReleased = "Fox::Event::Engine::Input::Mouse::MouseButtonReleased"_hash;
	const ParamId MouseMoved = "Fox::Event::Engine::Input::Mouse::MouseMoved"_hash;
}

namespace Fox::Event::Engine::Scene
{
	const EventId OnLoadScene = "Fox::Event::Engine::Scene::OnLoadScene"_hash;
	const ParamId LOADED_SCENE = "Fox::Event::Engine::Scene::LOADED_SCENE"_hash;
}