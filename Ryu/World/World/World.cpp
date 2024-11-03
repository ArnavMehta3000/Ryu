#include "World.h"
#include "Logger/Logger.h"

namespace Ryu::World
{
	void World::Init()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(World), "Initializing world");

		// Create all subsystems
		const auto& subsystems = Subsystem::GetData();
		for (auto& [name, subsystem] : subsystems)
		{
			if (auto ptr = Subsystem::Create(name, this))
			{
				LOG_TRACE(RYU_USE_LOG_CATEGORY(World), "Created world subsystem - {}", name);
				m_systems.push_back(std::move(ptr));
			}
		}

		// Initialize all subsystems
		for (auto& system : m_systems)
		{
			if (!system->Init())
			{
				LOG_WARN(RYU_USE_LOG_CATEGORY(World), "Failed to initialize world subsystem - {}", system->GetName());
			}
		}

		LOG_DEBUG(RYU_USE_LOG_CATEGORY(World), "World initialized");
	}

	void World::Shutdown()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(World), "Shutting down world");
		for (auto& system : m_systems)
		{
			system->Shutdown();
			system.reset();
		}

		m_systems.clear();

		LOG_DEBUG(RYU_USE_LOG_CATEGORY(World), "World shut down");
	}

	void World::Tick(f64 dt)
	{
		for (auto& system : m_systems)
		{
			// Only tick systems that are initialized
			UNLIKELY
			if (system->IsInitialized())
			{
				system->Tick(dt);
			}
		}
	}
}
