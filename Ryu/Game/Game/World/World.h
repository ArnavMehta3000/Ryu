#pragma once
#include "Common/Common.h"
#include "Utils/Serializer.h"
#include <entt/entity/registry.hpp>
#include <vector>
#include <tuple>

namespace Ryu::Game
{
	class Entity;
	
	using Registry         = entt::registry;
	using EntityHandle     = entt::entity;
	using EntityHandleType = entt::id_type;

	class World
	{
		friend class Entity;
	public:
		explicit World(const std::string& name) : m_name(name) {}
		~World() = default;

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity& entity);
		void DestroyEntityImmediate(const Entity& entity);
		Entity GetEntityByHandle(EntityHandle handle);

		u64 GetEntityCount() const;
		void ProcessPendingDestructions();
		u64 GetPendingDestructionCount() const;

		template <Utils::Serializable T> toml::table SerializeComponent(EntityHandle handle);
		template <Utils::Serializable... Ts> auto SerializeComponents(EntityHandle handle);

		template <Utils::Deserializable T> void DeserializeIntoExistingComponent(EntityHandle handle, const toml::table& table);

		inline NODISCARD auto GetAllEntities() const { return m_registry.view<entt::entity>(); }
		inline Registry& GetRegistry() { return m_registry; }
		inline const Registry& GetRegistry() const { return m_registry; }

	private:
		std::string m_name;
		Registry m_registry;
		std::vector<EntityHandle> m_pendingDestructions;
	};
}

#include "Game/World/World.inl"
