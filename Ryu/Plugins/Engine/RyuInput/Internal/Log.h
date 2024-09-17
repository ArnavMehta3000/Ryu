#pragma once
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

#define RYU_PLUGIN_INITIALIZE_LOGGER(name) RYU_CORE_LOG_INITIALIZE(name)
#define RYU_PLUGIN_SHUTDOWN_LOGGER()       RYU_CORE_LOG_SHUTDOWN()
#define RYU_PLUGIN_FATAL(...)              RYU_CORE_LOG_FATAL(RyuInput, __VA_ARGS__)
#define RYU_PLUGIN_ERROR(...)              RYU_CORE_LOG_ERROR(RyuInput, __VA_ARGS__)
#define RYU_PLUGIN_WARN(...)               RYU_CORE_LOG_WARN(RyuInput, __VA_ARGS__)
#define RYU_PLUGIN_INFO(...)               RYU_CORE_LOG_INFO(RyuInput, __VA_ARGS__)
#define RYU_PLUGIN_DEBUG(...)              RYU_CORE_LOG_DEBUG(RyuInput, __VA_ARGS__)
#define RYU_PLUGIN_TRACE(...)              RYU_CORE_LOG_TRACE(RyuInput, __VA_ARGS__)
#define RYU_PLUGIN_ASSERT(cond, message)   RYU_CORE_ASSERT(RyuInput, cond, message)