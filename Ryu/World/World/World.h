#pragma once
#include "World/Subsystem.h"
#include "Logger/LogCategory.h"
#include <vector>
#include <entt/entt.hpp>

namespace Ryu::World
{
	using EntityHandle = entt::handle;

	class World final
	{
		RYU_LOG_CATEGORY(World);
		RYU_DISABLE_COPY_AND_MOVE(World);
	
	public:
		World() = default;
		~World() = default;

		void Init();
		void Shutdown();
		void Tick(f64 dt);

	private:
		entt::registry m_registry;
		std::vector<std::unique_ptr<Subsystem>> m_systems;
	};
}
