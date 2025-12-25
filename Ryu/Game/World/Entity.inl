#define WorldCheck() RYU_ASSERT(m_world, "Entity is not attached to a world!")
#define DestroyCheck() RYU_ASSERT(!IsMarkedForDestroy(), "Entity is marked for destruction!")

namespace Ryu::Game
{
	template<typename T, typename ...Args>
	inline T& Entity::AddComponent(Args && ...args)
	{
		WorldCheck();
		DestroyCheck();
		return m_world->GetRegistry().emplace<T>(m_handle, std::forward<Args>(args)...);
	}

	template<typename T>
	inline T& Entity::GetComponent()
	{
		WorldCheck();
		return m_world->GetRegistry().get<T>(m_handle);
	}

	template<typename T>
	inline const T& Entity::GetComponent() const
	{
		WorldCheck();
		return m_world->GetRegistry().get<T>(m_handle);
	}

	template<typename T>
	inline T* Entity::TryGetComponent()
	{
		WorldCheck();
		return m_world->GetRegistry().try_get<T>(m_handle);
	}

	template<typename T>
	inline const T* Entity::TryGetComponent() const
	{
		WorldCheck();
		return m_world->GetRegistry().try_get<T>(m_handle);
	}

	template<typename T>
	inline bool Entity::HasComponent() const
	{
		return m_world && m_world->GetRegistry().all_of<T>(m_handle);
	}

	template<typename T>
	inline void Entity::RemoveComponent()
	{
		WorldCheck();
		m_world->GetRegistry().remove<T>(m_handle);
	}
}

#undef WorldCheck
