#include "Testbench/TestbenchApp.h"
#include "Engine/Engine.h"
#include "Profiling/Profiling.h"
#include "Game/World/Entity.h"
#include "Game/Components/Transform.h"

using namespace Ryu;
using namespace Ryu::Window;

TestbenchApp::TestbenchApp(std::shared_ptr<Ryu::Window::Window> window)
	: App::App(window)
	, m_world("TestbenchWorld")
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

	// m_world.CreateEntity("Player1").GetComponent<Game::Transform>().Position.z = 15.7;
	// m_world.CreateEntity("Player2").GetComponent<Game::Transform>().Position.x = 100.2;
	// TestSerialization();

	// TestDeserialization();

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
}

void TestbenchApp::TestSerialization()
{
	toml::table root;
	toml::array entitiesArray;

	m_world.GetAllEntities().each(
	[&](Game::EntityHandle handle)
	{
		auto [metadataTable, transformTable] = m_world.SerializeComponents<Game::EntityMetadata, Game::Transform>(handle);

		// Add components to table
		toml::table componentsTable;
		componentsTable.insert("metadata", std::move(metadataTable));
		componentsTable.insert("transform", std::move(transformTable));

		// Add components to entity
		toml::table entityTable;
		entityTable.insert("components", std::move(componentsTable));

		entitiesArray.push_back(std::move(entityTable));
	});

	root.insert("entities", std::move(entitiesArray));

	std::ofstream out("test.toml");
	out << root;
}

void TestbenchApp::TestDeserialization()
{
	try
	{
		toml::table root = toml::parse_file("test.toml");

		// Get all entities
		if (toml::array* entities = root["entities"].as_array())
		{
			for (auto& entityNode : *entities)
			{
				if (toml::table* entityTable = entityNode.as_table())
				{
					// Create the entity
					Game::Entity entity = m_world.CreateEntity();

					// Entity will already have the Metadata and Transform
					if (toml::table* components = (*entityTable)["components"].as_table())
					{
						if (toml::table* metadataTable = (*components)["metadata"].as_table())
						{
							m_world.DeserializeIntoExistingComponent<Game::EntityMetadata>(entity.GetHandle(), *metadataTable);
						}

						if (toml::table* transformTable = (*components)["transform"].as_table())
						{
							m_world.DeserializeIntoExistingComponent<Game::Transform>(entity.GetHandle(), *transformTable);
						}
					}
				}
			}
		}
	}
	catch (const toml::parse_error& e)
	{
		RYU_LOG_ERROR("Error parsing TOML file: {}", e.description().data());
	}
}
