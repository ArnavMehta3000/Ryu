#pragma once
#include "Game/Components/EntityMetadata.h"
#include "Game/World/World.h"

namespace Ryu::Game
{
	constexpr auto InvalidEntityHandle = entt::null;

	// Lightweight wrapper around World and entt entity
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
		World*       m_world = nullptr;
	};
}

#include "Game/World/Entity.inl"
