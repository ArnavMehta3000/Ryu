#pragma once
#include "Common/Enum.h"
#include "Utils/UUID.h"
#include "Game/World/World.h"
#include <bitset>
#include <string>

namespace Ryu::Game
{
	constexpr auto InvalidEntityHandle = entt::null;

	enum class EntityFlag
	{
		None             = 0,
		MarkedForDestroy = RYU_ENUM_BIT(0),
		Disabled         = RYU_ENUM_BIT(1),

		MAX_FLAGS = 32
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(EntityFlag)

	using EntityFlags = std::bitset<static_cast<u64>(EntityFlag::MAX_FLAGS)>;

	// Every entity is going to have this as a component
	struct EntityMetadata
	{
		EntityMetadata()
		{
			ID = Utils::UUID::Generate();
		}

		std::string Name;
		EntityFlags Flags;
		Utils::UUID::Type ID;
	};

	class Entity
	{
		friend class World;
	public:
		Entity() = default;
		Entity(EntityHandle handle, World* world) : m_handle(handle), m_world(world) {}

		template <typename T, typename... Args> T& AddComponent(Args&&...args);
		template <typename T> T&                   GetComponent();
		template <typename T> const T&             GetComponent() const;
		template <typename T> T*                   TryGetComponent();
		template <typename T> const T*             TryGetComponent() const;
		template <typename T> bool                 HasComponent() const;
		template <typename T> void                 RemoveComponent();

		bool IsValid() const;
		void Destroy();

		void SetFlag(EntityFlag flag, bool value = true);
		bool HasFlag(EntityFlag flag) const;
		void ClearFlag(EntityFlag flag);
		void ClearAllFlags();
		EntityFlag GetFlags() const;

		void MarkForDestroy();
		bool IsMarkedForDestroy() const;
		void SetEnabled(bool enabled);
		bool IsEnabled() const;

		bool operator==(const Entity& other) const
		{
			return m_handle == other.m_handle && m_world == other.m_world;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		inline EntityHandle GetHandle() const { return m_handle; }
		inline World* GetWorld() const { return m_world; }

	private:
		EntityHandle m_handle = InvalidEntityHandle;
		World* m_world = nullptr;
	};
}

#include "Game/World/Entity.inl"
