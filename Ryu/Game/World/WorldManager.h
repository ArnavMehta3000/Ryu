#pragma once
#include "Core/Utils/Timing/FrameTimer.h"
#include "Game/World/World.h"

namespace Ryu::Game
{
	class WorldManager
	{
	public:
		WorldManager() = default;
		~WorldManager();

		[[nodiscard]] inline World* GetActiveWorld() const { return m_activeWorld.get(); }

		template <IsWorld T, typename ...Args>
		void CreateWorld(Args&& ...args)
		{
			ShutdownWorld();
			m_activeWorld = std::make_unique<T>(std::forward<Args>(args)...);
			InitWorld();
		}

		void OnTick(const Utils::FrameTimer& timer);

	private:
		void InitWorld();
		void ShutdownWorld();

	private:
		std::unique_ptr<World> m_activeWorld;
	};
}
