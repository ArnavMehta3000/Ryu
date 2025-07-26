#include "Game/World/World.h"
#include "Game/World/Entity.h"
#include "Game/Components/Transform.h"
#include <entt/entity/registry.hpp>

namespace Ryu::Game
{
	Entity World::CreateEntity(const std::string& name)
	{
		EntityHandle handle = m_registry.create();		

		// Add metadata component
		auto& meta = m_registry.emplace<EntityMetadata>(handle);
		meta.Name = name.empty() ? std::format("Entity{}", GetEntityCount()) : name;

		// Add transform component
		m_registry.emplace<Transform>(handle);

		return Entity(handle, this);
	}
	
	void World::DestroyEntity(Entity& entity)
	{
		if (entity.IsValid() && entity.GetWorld() == this)
		{
			entity.MarkForDestroy();
			m_pendingDestructions.push_back(entity.GetHandle());
		}
	}
	
	void World::DestroyEntityImmediate(const Entity& entity)
	{
		if (entity.IsValid() && entity.GetWorld() == this)
		{
			m_registry.destroy(entity.GetHandle());
		}
	}
	
	Entity World::GetEntityByHandle(EntityHandle handle)
	{
		if (m_registry.valid(handle))
		{
			return Entity(handle, this);
		}
		
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
}
