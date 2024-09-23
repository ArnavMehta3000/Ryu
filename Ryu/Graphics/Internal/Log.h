#pragma once
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

#define RYU_GFX_INITIALIZE_LOGGER(name) RYU_CORE_LOG_INITIALIZE(name)
#define RYU_GFX_SHUTDOWN_LOGGER()       RYU_CORE_LOG_SHUTDOWN()
#define RYU_GFX_FATAL(...)              RYU_CORE_LOG_FATAL(Graphics, __VA_ARGS__)
#define RYU_GFX_ERROR(...)              RYU_CORE_LOG_ERROR(Graphics, __VA_ARGS__)
#define RYU_GFX_WARN(...)               RYU_CORE_LOG_WARN(Graphics, __VA_ARGS__)
#define RYU_GFX_INFO(...)               RYU_CORE_LOG_INFO(Graphics, __VA_ARGS__)
#define RYU_GFX_DEBUG(...)              RYU_CORE_LOG_DEBUG(Graphics, __VA_ARGS__)
#define RYU_GFX_TRACE(...)              RYU_CORE_LOG_TRACE(Graphics, __VA_ARGS__)
#define RYU_GFX_ASSERT(cond, message)   RYU_CORE_ASSERT(Graphics, cond, message)

#define RYU_GFX_ASSERTHR(hr, message)\
{                                    \
	if (FAILED(hr))                  \
	{                                \
		RYU_GFX_FATAL(message, hr);  \
	}                                \
}
