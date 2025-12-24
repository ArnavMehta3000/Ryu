#pragma once
#include "Core/Utils/Timing/FrameTimer.h"
#include "Application/Event/ListenerHandle.h"
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

#if defined(RYU_WITH_EDITOR)
		void OnImGuiRender();  // Should be called by the editor
#endif

	private:
		void InitWorld();
		void ShutdownWorld();
		void BindWorldEvents();
		void UnbindWorldEvents();

		void OnWorldCreationBegin(World& world);
		void OnWorldCreationEnd(World& world);
		void OnWorldDestructionBegin(World& world);
		void OnWorldDestructionEnd(World& world);

	private:
		std::unique_ptr<World> m_activeWorld;

		Event::ListenerHandle m_onWorldCreationBeginEventHandle;
		Event::ListenerHandle m_onWorldCreationEndEventHandle;
		Event::ListenerHandle m_onWorldDestructionBeginEventHandle;
		Event::ListenerHandle m_onWorldDestructionEndEventHandle;
	};
}
