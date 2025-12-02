#include "Testbench/TestbenchApp.h"
#include "Testbench/TestbenchWorld.h"
#include "Engine/Engine.h"
#include "Profiling/Profiling.h"
#include "Game/World/Entity.h"
#include "Game/Components/Transform.h"
#include "Utils/ReflectionSerializer.h"
#include "App/Utils/PathManager.h"

using namespace Ryu;
using namespace Ryu::Window;

TestbenchApp::TestbenchApp(const std::shared_ptr<Ryu::Window::Window>& window)
	: App::App(window)
	, m_keyListener(window->GetDispatcher(), [this](const KeyEvent& e)
	{
		if (e.KeyCode == KeyCode::Escape && e.State == KeyState::Released)
		{
#if defined(RYU_BUILD_DEBUG) && !defined(RYU_WITH_EDITOR)
			Quit();  // Only close window from here when we are in debug and not in editor
#endif
		}

		if (e.KeyCode == KeyCode::F && e.State == KeyState::Released)
		{
			RYU_LOG_INFO("DT: {} | FPS: {} | Total: {:3f} | Frame: {}",
				m_timeInfo.DeltaTime, m_timeInfo.FPS, m_timeInfo.TotalTime, m_timeInfo.FrameCount);
		}
	})
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

void TestbenchApp::OnTick(const Ryu::Utils::TimeInfo& t)
{
	m_gameInput.PollKeyboard();
	m_gameInput.PollMouse();
	
	m_timeInfo = t;

	m_worldManager.OnTick(t);
}

void TestbenchApp::TestSerialization()
{
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

	auto outDir = Ryu::App::PathManager::Get().GetProjectDir() / "saved.toml";
	std::ofstream out(outDir.string());
	out << root;
}
