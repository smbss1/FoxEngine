
#pragma once

#include "Core/Types.hpp"

namespace FoxEvent::Engine
{
	const ParamId ENTITY = "Fox::Engine::ENTITY"_hash;
	const EventId OnStartGame = CreateEventId("Fox::Engine::OnStartGame");
}

namespace FoxEvent::Engine::Window
{
	const EventId OnRender = "FoxEvent::Engine::Window::OnRender"_hash;
	const EventId OnInput = "FoxEvent::Engine::Window::OnInput"_hash;

	const ParamId INPUT = "FoxEvent::Engine::Window::INPUT"_hash;
	const ParamId RENDER_WINDOW = "FoxEvent::Engine::Window::RENDER_WINDOW"_hash;
}

namespace FoxEvent::Engine::Scene
{
	const EventId OnLoadScene = "FoxEvent::Engine::Scene::OnLoadScene"_hash;
	const ParamId LOADED_SCENE = "FoxEvent::Engine::Scene::LOADED_SCENE"_hash;
}