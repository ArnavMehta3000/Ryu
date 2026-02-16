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
	
	// Create camera
	m_mainCamera = CreateEntity("Main Camera");
	Game::CameraComponent& cam = m_mainCamera.AddComponent<Game::CameraComponent>();
	cam.Mode = Game::CameraComponent::Projection::Orthographic;

	// Move the camera
	Game::Transform& camTransform = m_mainCamera.GetComponent<Game::Transform>();
	camTransform.Position = { 0.0f, 0.0f, 15.0f };

	// Create mesh
	m_meshEntity = CreateEntity("Mesh");
	Game::MeshRenderer& mr = m_meshEntity.AddComponent<Game::MeshRenderer>();
	mr.MeshHandle = mr.GetAssetRegistry()->GetPrimitive(Asset::PrimitiveType::Sphere);

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
