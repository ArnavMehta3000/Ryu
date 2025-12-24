#include "Game/World/World.h"
#include "Game/World/Entity.h"
#include "Game/Components/Transform.h"
#include "Core/Logging/Logger.h"
#include <entt/entity/registry.hpp>

namespace Ryu::Game
{
	Entity World::CreateEntity(const std::string& name)
	{
		EntityHandle handle = m_registry.create();		

		// Add metadata component
		auto& meta = m_registry.emplace<EntityMetadata>(handle);
		meta.Name = name.empty() ? fmt::format("Entity{}", GetEntityCount()) : name;

		// Add transform component
		m_registry.emplace<Transform>(handle);

		RYU_LOG_TRACE("Created entity {}", name);

		return Entity(handle, this);
	}
	
	void World::DestroyEntity(Entity& entity)
	{
		if (entity.IsValid() && entity.GetWorld() == this)
		{
			entity.MarkForDestroy();
			m_pendingDestructions.push_back(entity.GetHandle());
		}
		else
		{
			RYU_LOG_WARN("Tried to destroy invalid entity");
		}
	}
	
	void World::DestroyEntityImmediate(const Entity& entity)
	{
		if (entity.IsValid() && entity.GetWorld() == this)
		{
			m_registry.destroy(entity.GetHandle());
		}
		else
		{
			RYU_LOG_WARN("Tried to immediately destroy invalid entity");
		}
	}
	
	Entity World::GetEntityByHandle(EntityHandle handle)
	{
		if (m_registry.valid(handle))
		{
			return Entity(handle, this);
		}
		
		RYU_LOG_WARN("Tried to get entity with an invalid handle (id:{})", (EntityHandleType)handle);
		return Entity();
	}
	
	u64 World::GetEntityCount() const
	{
		return m_registry.view<entt::entity>().size();
	}

	void World::ProcessPendingDestructions()
	{
		if (m_pendingDestructions.empty())
		{
			return;
		}

		// Remove duplicates
		std::sort(m_pendingDestructions.begin(), m_pendingDestructions.end());
		m_pendingDestructions.erase(
			std::unique(m_pendingDestructions.begin(), m_pendingDestructions.end()),
			m_pendingDestructions.end()
		);

		// Destroy entities
		for (auto handle : m_pendingDestructions) 
		{
			if (m_registry.valid(handle))
			{
				m_registry.destroy(handle);
			}
		}

		m_pendingDestructions.clear();
	}
	
	u64 World::GetPendingDestructionCount() const
	{
		return m_pendingDestructions.size();
	}
	
	void World::OnCreate() { }
	
	void World::OnDestroy() { }

	void World::OnTick(const Utils::FrameTimer& ) { }
	
#if defined(RYU_WITH_EDITOR)
	void World::OnImGuiRender() { }
#endif
}
