#pragma once
#include "Game/World/WorldEvents.h"
#include "Common/Common.h"
#include "Utils/Serializer.h"
#include "Utils/Timing/FrameTimer.h"
#include "Event/EventEmitter.h"
#include <entt/entity/registry.hpp>
#include <vector>

namespace Ryu::Game
{
	class Entity;
	class WorldManager;
	
	using Registry         = entt::registry;
	using EntityHandle     = entt::entity;
	using EntityHandleType = entt::id_type;

	class World : public Event::EventEmitter
	{
		friend class Entity;
		friend class WorldManager;
	public:
		virtual ~World() = default;

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

		[[nodiscard]] inline auto GetAllEntities() const { return m_registry.view<entt::entity>(); }
		[[nodiscard]] inline Registry& GetRegistry() { return m_registry; }
		[[nodiscard]] inline const Registry& GetRegistry() const { return m_registry; }
		[[nodiscard]] inline WorldManager* GetWorldManager() const { return m_worldManager; }
		[[nodiscard]] inline const std::string& GetName() const { return m_name; }
	
	protected:
		explicit World(const std::string& name) : m_name(name) {}

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void OnTick(const Utils::FrameTimer& timer);

#if defined(RYU_WITH_EDITOR)
		virtual void OnImGuiRender();  // Should be invoked by WorldManager
#endif

	private:
		WorldManager*             m_worldManager = nullptr;
		std::string               m_name;
		Registry                  m_registry;
		std::vector<EntityHandle> m_pendingDestructions;
	};

	template <typename T>
	concept IsWorld = std::is_base_of_v<World, T>;
}

#include "Game/World/World.inl"
