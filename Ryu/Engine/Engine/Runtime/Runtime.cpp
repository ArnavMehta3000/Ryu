#include "Runtime.h"
#include "Logger/Logger.h"

namespace Ryu::Engine
{
	Runtime::Runtime()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Runtime created");
	}

	bool Runtime::OnInit()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Initializing Runtime");

		return true;
	}
	
	void Runtime::OnShutdown()
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Runtime), "Shutting down Runtime");

	}

	void Runtime::OnTick(f64)
	{
	}
}
