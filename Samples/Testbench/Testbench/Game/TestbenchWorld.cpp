#include "Testbench/Game/TestbenchWorld.h"

#include "Asset/Primitives.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiling/Profiling.h"
#include "Engine/Engine.h"
#include "Game/Components/CameraComponent.h"
#include "Game/Components/MeshRenderer.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Core/GameRuntime.h"
#include <ImGui/imgui.h>

// NOTE: Event listeners are broken if we hot-reload
// TODO: OnTick is not yet called (triggerred by world manager?)


void TestbenchWorld::OnCreate()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_DEBUG("Testbench World Created");
	using namespace Ryu;
	using Runtime = RYU_RUNTIME();

	m_inputManager = Runtime::GetInputManager();
	Ryu::Window::WindowSizePos size = Runtime::GetApplication()->GetWindow()->GetSize();
	
	// Create camera
	m_mainCamera = CreateEntity("Main Camera");
	Game::CameraComponent& cam = m_mainCamera.AddComponent<Game::CameraComponent>();
	cam.Mode = Game::CameraComponent::Projection::Orthographic;

	// Move the camera
	Game::Transform& camTransform = m_mainCamera.GetComponent<Game::Transform>();
	camTransform.Position = { 0.0f, 0.0f, 15.0f };

	// Create mesh
	m_meshEntity = CreateEntity("Mesh");
	m_meshEntity.AddComponent<Game::MeshRenderer>(
		Runtime::GetRenderer()->GetAssetRegistry(),
		Asset::PrimitiveType::Sphere);

	m_inputManager->BindAction("ToggleWireframe", Ryu::Window::KeyCode::R, []()
	{
		Gfx::WorldRenderer* renderer = Runtime::GetRenderer()->GetWorldRenderer();
		
		auto config = renderer->GetConfig();
		config.EnableWireframe = !config.EnableWireframe;
		
		renderer->SetConfig(config);
	});

	m_inputManager->BindAction("ToggleCameraProjection", Ryu::Window::KeyCode::C, [&cam]()
	{
		cam.Mode = cam.Mode == Game::CameraComponent::Projection::Orthographic 
			? Game::CameraComponent::Projection::Perspective 
			: Game::CameraComponent::Projection::Orthographic;
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
		camera.OrthoSize[0] -= speed;
		camera.OrthoSize[1] -= speed;
	}
	if (m_inputManager->IsKeyDown(KC::X))
	{
		camera.FovY += speed;
		camera.OrthoSize[0] += speed;
		camera.OrthoSize[1] += speed;
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

			auto& camTransform = m_mainCamera.GetComponent<Ryu::Game::Transform>();
			auto eulerRot = camTransform.Orientation.ToEuler();
			ImGui::Text("Camera fov: %.2f", m_mainCamera.GetComponent<Ryu::Game::CameraComponent>().FovY);
			ImGui::DragFloat3("Camera Position", &camTransform.Position.x, 0.1f);
			ImGui::DragFloat3("Camera Rotation", &eulerRot.x, 0.1f);
			camTransform.Orientation = Ryu::Math::Quaternion::CreateFromYawPitchRoll(eulerRot);

			auto& cubeTransform = m_meshEntity.GetComponent<Ryu::Game::Transform>();
			ImGui::DragFloat3("Cube Position", &cubeTransform.Position.x, 0.1f);

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
