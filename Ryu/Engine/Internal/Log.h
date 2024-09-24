#pragma once
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

#define RYU_ENGINE_INITIALIZE_LOGGER(name) RYU_CORE_LOG_INITIALIZE(name)
#define RYU_ENGINE_SHUTDOWN_LOGGER()       RYU_CORE_LOG_SHUTDOWN()
#define RYU_ENGINE_FATAL(...)              RYU_CORE_LOG_FATAL(Engine, __VA_ARGS__)
#define RYU_ENGINE_ERROR(...)              RYU_CORE_LOG_ERROR(Engine, __VA_ARGS__)
#define RYU_ENGINE_WARN(...)               RYU_CORE_LOG_WARN(Engine, __VA_ARGS__)
#define RYU_ENGINE_INFO(...)               RYU_CORE_LOG_INFO(Engine, __VA_ARGS__)
#define RYU_ENGINE_DEBUG(...)              RYU_CORE_LOG_DEBUG(Engine, __VA_ARGS__)
#define RYU_ENGINE_TRACE(...)              RYU_CORE_LOG_TRACE(Engine, __VA_ARGS__)
#define RYU_ENGINE_ASSERT(cond, ...)       RYU_CORE_ASSERT(Engine, cond, __VA_ARGS__)
