#pragma once
#include "Common/Common.h"
#include <entt/entity/registry.hpp>
#include <vector>

namespace Ryu::Game
{
	class Entity;

	using EntityHandle = entt::entity;
	using Registry = entt::registry;

	class World
	{
		friend class Entity;
	public:
		World() = default;
		~World() = default;

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity& entity);
		void DestroyEntityImmediate(Entity& entity);
		Entity GetEntityByHandle(EntityHandle handle);

		u64 GetEntityCount() const;
		void ProcessPendingDestructions();
		u64 GetPendingDestructionCount() const;

		inline Registry& GetRegistry() { return m_registry; }
		inline const Registry& GetRegistry() const { return m_registry; }

	private:
		Registry m_registry;
		std::vector<EntityHandle> m_pendingDestructions;
	};
}
