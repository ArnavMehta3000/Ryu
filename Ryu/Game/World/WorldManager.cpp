#include "Game/World/WorldManager.h"
#include "Core/Logging/Logger.h"

namespace Ryu::Game
{
	WorldManager::~WorldManager()
	{
		ShutdownWorld();
	}

	void WorldManager::OnTick(const Utils::FrameTimer& timer)
	{
		if (m_activeWorld)
		{
			// Cleanup anything that was marked for destruction in the previous frame
			m_activeWorld->ProcessPendingDestructions();

			m_activeWorld->OnTick(timer);
		}
	}

#if defined(RYU_WITH_EDITOR)
	void WorldManager::OnImGuiRender()
	{
		if (m_activeWorld)
		{
			m_activeWorld->OnImGuiRender();
		}
	}
#endif

	void WorldManager::InitWorld()
	{
		if (m_activeWorld)
		{

			m_activeWorld->OnCreate();
		}
	}
	
	void WorldManager::ShutdownWorld()
	{
		if (m_activeWorld)
		{
			m_activeWorld->ProcessPendingDestructions();
			m_activeWorld->OnDestroy();
		}
	}
}
