#include "IWorldSubsystem.h"
#include "World/World.h"

namespace Ryu::World
{
	IWorldSubsystem::IWorldSubsystem(World* world)
		: m_world(world)
		, m_canTick(true)
		, m_isInitialized(false)
	{
	}

	inline constexpr RYU_API World* IWorldSubsystem::GetWorld() const noexcept
	{
		return m_world;
	}

	constexpr bool IWorldSubsystem::CanTick() const noexcept
	{
		return m_canTick;
	}

	constexpr void IWorldSubsystem::CanTick(bool canTick) noexcept
	{
		m_canTick = canTick;
	}

	bool IWorldSubsystem::Init()
	{
		m_isInitialized = OnInit();
		return m_isInitialized;
	}

	void IWorldSubsystem::Shutdown()
	{
		OnShutdown();
		m_isInitialized = false;
	}

	void IWorldSubsystem::Tick(f64 dt)
	{
		if (m_canTick)
		{
			OnTick(dt);
		}
	}

	entt::registry& IWorldSubsystem::GetRegistry() const noexcept
	{
		return GetWorld()->m_registry;
	}
}
