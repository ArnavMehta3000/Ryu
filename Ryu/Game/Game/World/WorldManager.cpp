#include "Game/World/WorldManager.h"
#include "Logging/Logger.h"

namespace Ryu::Game
{
	WorldManager::~WorldManager()
	{
		ShutdownWorld();
	}

	void WorldManager::InitWorld()
	{
		if (m_activeWorld)
		{
			BindWorldEvents();

			m_activeWorld->Emit(WorldCreationBeginEvent{});
			m_activeWorld->OnCreate();
			m_activeWorld->Emit(WorldCreationEndEvent{});
		}
	}
	
	void WorldManager::ShutdownWorld()
	{
		if (m_activeWorld)
		{
			m_activeWorld->ProcessPendingDestructions();

			m_activeWorld->Emit(WorldDestructionBeginEvent{});
			m_activeWorld->OnDestroy();
			m_activeWorld->Emit(WorldDestructionEndEvent{});
		}
	}
	
	void WorldManager::BindWorldEvents()
	{
		if (m_activeWorld)
		{
			m_onWorldCreationBeginEventHandle    = m_activeWorld->On<WorldCreationBeginEvent>([this](const WorldCreationBeginEvent&)       { OnWorldCreationBegin(*m_activeWorld);    });
			m_onWorldCreationEndEventHandle      = m_activeWorld->On<WorldCreationEndEvent>([this](const WorldCreationEndEvent&)           { OnWorldCreationEnd(*m_activeWorld);      });
			m_onWorldDestructionBeginEventHandle = m_activeWorld->On<WorldDestructionBeginEvent>([this](const WorldDestructionBeginEvent&) { OnWorldDestructionBegin(*m_activeWorld); });
			m_onWorldDestructionEndEventHandle   = m_activeWorld->On<WorldDestructionEndEvent>([this](const WorldDestructionEndEvent&)     { OnWorldDestructionEnd(*m_activeWorld);   });
		}
	}

	void WorldManager::UnbindWorldEvents()
	{
		if (m_activeWorld)
		{
			if (m_onWorldCreationBeginEventHandle) 
				m_activeWorld->Unsubscribe(m_onWorldCreationBeginEventHandle);
			if (m_onWorldCreationEndEventHandle) 
				m_activeWorld->Unsubscribe(m_onWorldCreationEndEventHandle);
			if (m_onWorldDestructionBeginEventHandle) 
				m_activeWorld->Unsubscribe(m_onWorldDestructionBeginEventHandle);
			if (m_onWorldDestructionEndEventHandle) 
				m_activeWorld->Unsubscribe(m_onWorldDestructionEndEventHandle);
		}
	}
	
	void WorldManager::OnWorldCreationBegin(World& world)
	{
		RYU_LOG_DEBUG("On world ({}) creation begin", world.GetName());
	}
	
	void WorldManager::OnWorldCreationEnd(World& world)
	{
		RYU_LOG_DEBUG("On world ({}) creation end", world.GetName());
	}
	
	void WorldManager::OnWorldDestructionBegin(World& world)
	{
		RYU_LOG_DEBUG("On world ({}) destruction begin", world.GetName());
	}
	
	void WorldManager::OnWorldDestructionEnd(World& world)
	{
		RYU_LOG_DEBUG("On world ({}) destruction end", world.GetName());
	}
}
