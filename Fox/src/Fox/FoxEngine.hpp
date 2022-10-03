
#ifndef FOX_ENGINE_HPP_
#define FOX_ENGINE_HPP_

#include "Core/Base.hpp"

#include "Core/Application.hpp"
#include "Core/State.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Assert.hpp"
#include "Core/Managers/StateMachine.hpp"

#include "Time.hpp"

#include "Core/Input/Input.hpp"
#include "Core/Input/KeyCodes.hpp"
#include "Core/Input/MouseCodes.hpp"
#include "Core/Input/MouseCodes.hpp"
#include "Renderer/OrthographicCameraController.hpp"

#include "ImGui/ImGuiState.hpp"

#include "Core/Scene.hpp"
#include "Ecs/Entity.hpp"
#include "Components/ScriptableBehaviour.hpp"
#include "Components.hpp"

// ---Renderer------------------------
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/RendererCommand.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Framebuffer.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/VertexArray.hpp"

#include "Renderer/OrthographicCamera.hpp"

#endif
