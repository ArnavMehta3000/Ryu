#include "Testbench/TestbenchWorld.h"

#include "Asset/Primitives.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiling/Profiling.h"
#include "Engine/Engine.h"
#include "Game/Components/CameraComponent.h"
#include "Game/Components/MeshRenderer.h"
#include "Game/Components/TransformComponent.h"
#include "Game/World/Entity.h"
#include <ImGui/imgui.h>

void TestbenchWorld::OnCreate()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_DEBUG("Testbench World Created");
	using namespace Ryu;

	m_inputManager = Engine::Engine::Get().GetInputManager();
	Ryu::Window::WindowSizePos size = Engine::Engine::Get().GetAppWindow()->GetSize();
	
	// Create camera
	m_mainCamera = CreateEntity("Main Camera");
	Game::CameraComponent& cam = m_mainCamera.AddComponent<Game::CameraComponent>();
	// The camera viewport should be set externally somehow
	// Or should it be a user side thing? (prolly not tbh)
	cam.Viewport.width         = (f32)size.X;
	cam.Viewport.height        = (f32)size.Y;

	RYU_TODO("Camera viewport should be controlled by the engine when the window is resized");

	// Move the camera
	Game::Transform& camTransform = m_mainCamera.GetComponent<Game::Transform>();
	camTransform.Position = { 0.0f, 0.0f, -10.0f };

	// Create mesh
	m_meshEntity = CreateEntity("Mesh");
	m_meshEntity.AddComponent<Game::MeshRenderer>(
		Engine::Engine::Get().GetRenderer()->GetAssetRegistry(),
		Asset::PrimitiveType::Cube);

	m_inputManager->BindAction("ToggleWireframe", Ryu::Window::KeyCode::R, []()
	{
		Gfx::WorldRenderer* renderer = Engine::Engine::Get().GetRenderer()->GetWorldRenderer();
		
		auto config = renderer->GetConfig();
		config.EnableWireframe = !config.EnableWireframe;
		
		renderer->SetConfig(config);
	});
}

void TestbenchWorld::OnDestroy()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_DEBUG("Testbench World Destroyed");
}

f32 t = 0.0f;
void TestbenchWorld::OnTick(const Ryu::Utils::FrameTimer& timer)
{
	RYU_PROFILE_SCOPE();
	using namespace Ryu;
	using KC = Ryu::Window::KeyCode;

	m_timer = timer;

	t += timer.DeltaTimeF() * 1.5f;

	// Spin the object
	Game::Transform& meshTransform = m_meshEntity.GetComponent<Game::Transform>();
	meshTransform.Orientation = Math::Quaternion::CreateFromYawPitchRoll(Math::Vector3(t, t, t));

	// Update the camera
	const f32 speed = 10 * timer.DeltaTimeF();
	Game::CameraComponent& camera = m_mainCamera.GetComponent<Game::CameraComponent>();
	if (m_inputManager->IsKeyDown(KC::Z))
	{
		camera.FovY -= speed;
	}
	if (m_inputManager->IsKeyDown(KC::X))
	{
		camera.FovY += speed;
	}
	
	// Move the object around
	if (m_inputManager->IsKeyDown(KC::ArrowLeft))
	{
		meshTransform.Position = { meshTransform.Position.x + speed, meshTransform.Position.y, meshTransform.Position.z };
	}
	if (m_inputManager->IsKeyDown(KC::ArrowRight))
	{
		meshTransform.Position = { meshTransform.Position.x - speed, meshTransform.Position.y, meshTransform.Position.z };
	}
	if (m_inputManager->IsKeyDown(KC::ArrowUp))
	{
		meshTransform.Position = { meshTransform.Position.x, meshTransform.Position.y + speed, meshTransform.Position.z };
	}
	if (m_inputManager->IsKeyDown(KC::ArrowDown))
	{
		meshTransform.Position = { meshTransform.Position.x, meshTransform.Position.y - speed, meshTransform.Position.z };
	}
}

#if defined(RYU_WITH_EDITOR)
void TestbenchWorld::OnImGuiRender()
{
	RYU_PROFILE_SCOPE();
	// Draw debug info overlay
	{
		static int location = 1;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoFocusOnAppearing
			| ImGuiWindowFlags_NoNav;

		if (location >= 0)
		{
			ImVec2 windowPos, windowPosPivot;

			constexpr f32 PAD = 10.0f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 workPos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 workSize = viewport->WorkSize;

			windowPos.x      = (location & 1) ? (workPos.x + workSize.x - PAD) : (workPos.x + PAD);
			windowPos.y      = (location & 2) ? (workPos.y + workSize.y - PAD) : (workPos.y + PAD);
			windowPosPivot.x = (location & 1) ? 1.0f : 0.0f;
			windowPosPivot.y = (location & 2) ? 1.0f : 0.0f;

			ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
			ImGui::SetNextWindowViewport(viewport->ID);
			windowFlags |= ImGuiWindowFlags_NoMove;
		}
		else if (location == -2)
		{
			// Center window
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			windowFlags |= ImGuiWindowFlags_NoMove;
		}

		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

		if (ImGui::Begin("Info", nullptr, windowFlags))
		{
			ImGui::Text("World Info");
			ImGui::Separator();
			ImGui::Text("Delta Time: %.3fms", m_timer.DeltaTimeF() * 1000.0f);
			ImGui::Text("FPS: %.2f", m_timer.FPS());
			ImGui::Text("Total Time: %.2f", m_timer.TimeSinceStart<std::chrono::seconds>());
			ImGui::Text("Frame Count: %llu", m_timer.FrameCount());

			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom"      , NULL, location == -1)) location = -1;
				if (ImGui::MenuItem("Center"      , NULL, location == -2)) location = -2;
				if (ImGui::MenuItem("Top-left"    , NULL, location ==  0)) location = 0;
				if (ImGui::MenuItem("Top-right"   , NULL, location ==  1)) location = 1;
				if (ImGui::MenuItem("Bottom-left" , NULL, location ==  2)) location = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, location ==  3)) location = 3;

				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}
}
#endif
