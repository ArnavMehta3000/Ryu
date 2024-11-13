#include "Runtime.h"
#include "Logger/Logger.h"

namespace Ryu::Engine
{

	Runtime::Runtime()
		: m_world()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Runtime created");
	}

	bool Runtime::OnInit()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Initializing Runtime");

		// World initialization is handled by the engine
		// Since we want to initialize the world after the 
		// application has been initialized.
		// This is because the world is a member of the runtime

		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Runtime), "Runtime initialized");
		return true;
	}
	
	void Runtime::OnShutdown()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Shutting down Runtime");

		m_world.Shutdown();
		
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Runtime), "Runtime shut down");
	}
	
	void Runtime::OnTick(f64 dt)
	{
		m_world.Tick(dt);		
	}
}
