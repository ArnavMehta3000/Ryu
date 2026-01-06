#include "Testbench/TestbenchApp.h"

#include "Application/App/Utils/PathManager.h"
#include "Core/Profiling/Profiling.h"
#include "Core/Utils/ReflectionSerializer.h"
#include "Engine/Engine.h"
#include "Game/Components/TransformComponent.h"
#include "Game/World/Entity.h"
#include "Testbench/TestbenchWorld.h"

using namespace Ryu;
using namespace Ryu::Window;

TestbenchApp::TestbenchApp(const std::shared_ptr<Ryu::Window::Window>& window)
	: App::App(window)
{
}

bool TestbenchApp::OnInit()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO("Initializing Testbench App");

	if (!m_gameInput.Init())
	{
		RYU_LOG_WARN("Failed to initialize GameInput");
	}

	// Bind input stuff
	Game::InputManager* input = Engine::Engine::Get().GetInputManager();
	input->BindAction("Quit", KeyCode::Escape, [this]{ Quit(); });
	input->BindAction("ToggleStats", KeyCode::F, [this]{ RYU_LOG_INFO("DT: {} | FPS: {}", m_timer.DeltaTimeF(), m_timer.FPS()); });

	m_worldManager.CreateWorld<TestbenchWorld>();

	TestSerialization();

	return true;
}

void TestbenchApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO("Shutting down Testbench App");

	m_gameInput.Shutdown();
}

void TestbenchApp::OnTick(const Ryu::Utils::FrameTimer& timer)
{
	RYU_PROFILE_SCOPE();
	m_gameInput.PollKeyboard();
	m_gameInput.PollMouse();

	m_timer = timer;

	m_worldManager.OnTick(timer);
}

void TestbenchApp::TestSerialization()
{
	RYU_PROFILE_SCOPE();
	Game::World* world = m_worldManager.GetActiveWorld();
	world->CreateEntity("Player1").GetComponent<Game::Transform>().Position.z = 15.7f;
	world->CreateEntity("Player2").GetComponent<Game::Transform>().Position.x = 100.2f;

	toml::table root;
	toml::array entitiesArray;

	world->GetAllEntities().each(
	[&](Game::EntityHandle handle)
	{
		auto [metadataTable, transformTable] = world->SerializeComponents<Game::EntityMetadata, Game::Transform>(handle);

		toml::table componentsTable;
		componentsTable.insert("metadata", std::move(metadataTable));
		componentsTable.insert("transform", std::move(transformTable));

		toml::table entityTable;
		entityTable.insert("components", std::move(componentsTable));

		entitiesArray.push_back(std::move(entityTable));
	});
	root.insert("entities", std::move(entitiesArray));

	//auto outDir = Ryu::App::PathManager::Get().GetProjectDir() / "saved.toml";
	//std::ofstream out(outDir.string());
	//out << root;
}
