#pragma once
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

#define RYU_EDITOR_INITIALIZE_LOGGER(name) RYU_CORE_LOG_INITIALIZE(name)
#define RYU_EDITOR_SHUTDOWN_LOGGER()       RYU_CORE_LOG_SHUTDOWN()
#define RYU_EDITOR_FATAL(...)              RYU_CORE_LOG_FATAL(Editor, __VA_ARGS__)
#define RYU_EDITOR_ERROR(...)              RYU_CORE_LOG_ERROR(Editor, __VA_ARGS__)
#define RYU_EDITOR_WARN(...)               RYU_CORE_LOG_WARN(Editor, __VA_ARGS__)
#define RYU_EDITOR_INFO(...)               RYU_CORE_LOG_INFO(Editor, __VA_ARGS__)
#define RYU_EDITOR_DEBUG(...)              RYU_CORE_LOG_DEBUG(Editor, __VA_ARGS__)
#define RYU_EDITOR_TRACE(...)              RYU_CORE_LOG_TRACE(Editor, __VA_ARGS__)
#define RYU_EDITOR_ASSERT(cond, message)   RYU_CORE_ASSERT(Editor, cond, message)