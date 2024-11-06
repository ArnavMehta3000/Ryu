#pragma once
#include "Common/API.h"
#include <entt/entt.hpp>

namespace Ryu::World
{
	class World;

	/**
	 * @brief World subsystem interface
	 * @details Acts like an accessor interface to the owning world and its registry
	 */
	class IWorldSubsystem
	{
	public:
		/**
		 * @brief Get a pointer to the owning world
		 * @return Pointer to the owning world
		 */
		inline constexpr RYU_API World* GetWorld() const noexcept { return m_world; }
		
	protected:
		inline void SetWorld(World* world) noexcept { m_world = world; }
		entt::registry& GetRegistry() const noexcept;  // Implemented in World/Subsystem.cpp

	private:
		World* m_world;
	};
}
