#pragma once
#include "Common/Globals.h"
#include "Logger/Assert.h"
#include "Config/ConfigManager.h"
#include "App/Application.h"
#include "Engine/Engine.h"
#include "Engine/Entry/EngineMain.h"
#include "Profiling/Profiling.h"
#include "App/Utils/PathManager.h"

#if !defined(RYU_GAME_AS_DLL)
// Define the extern function to create the runtime
// Only enabled when game is not built as a DLL
#define RYU_USE_APP(RuntimeClass, ...)       \
::Ryu::App::App* CreateApplication()         \
{                                            \
	return new RuntimeClass(__VA_ARGS__);    \
}
#else
#define RYU_USE_APP(RuntimeClass, ...)
#endif
