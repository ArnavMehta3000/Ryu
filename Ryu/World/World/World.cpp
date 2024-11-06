#include "World.h"
#include "Logger/Logger.h"

namespace Ryu::World
{
	void World::Init()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(World), "Initializing world");

		// Create all subsystems using the factory
		const auto& subsystems = Subsystem::GetFactoryData();
		for (auto& [name, subsystem] : subsystems)
		{
			if (std::unique_ptr<Subsystem> ptr = Subsystem::Create(name, this))
			{
				LOG_TRACE(RYU_USE_LOG_CATEGORY(World), "Created world subsystem - {}", name);
				m_subsystems.push_back(std::move(ptr));
			}
		}

		// Initialize all subsystems
		for (auto& system : m_subsystems)
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
		for (auto& system : m_subsystems)
		{
			system->Shutdown();
		}

		// Split shutdown and deletion
		for (auto& system : m_subsystems)
		{
			system.reset();
		}

		m_subsystems.clear();

		// Clear all entities
		m_registry.clear();

		LOG_DEBUG(RYU_USE_LOG_CATEGORY(World), "World shut down");
	}

	void World::Tick(f64 dt)
	{
		// Only tick systems that are initialized
		for (auto& system : m_subsystems)
		{
			UNLIKELY
			if (system->IsInitialized())
			{
				system->Tick(dt);
			}
		}

		// Destroy all entities marked for deletion
		if (m_entitiesMarkedForDeletion.size() > 0)
		{
			for (auto& entity : m_entitiesMarkedForDeletion)
			{
				m_registry.destroy(entity);
			}
			m_entitiesMarkedForDeletion.clear();
		}
	}
	EntityHandle World::CreateEntity()
	{
		EntityHandle e = entt::handle(m_registry, m_registry.create());
		OnEntityCreate.Broadcast(e);

		LOG_TRACE(RYU_USE_LOG_CATEGORY(World), "Created entity with id - {}", entt::id_type(e.entity()));

		return e;
	}

	void World::DestroyEntity(const EntityHandle& entity)
	{
		// Do not destroy the entity instantly
		// Destroy at the end of the frame
		LOG_TRACE(RYU_USE_LOG_CATEGORY(World), "Entity marked for deletion - {}", entt::id_type(entity.entity()));
		m_entitiesMarkedForDeletion.push_back(entity.entity());
	}

	u64 World::GetAliveEntityCount() const
	{
		return m_registry.storage<entt::entity>()->free_list();
	}
}
