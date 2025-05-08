#pragma once
#include "Engine/Engine.h"
#include "App/Application.h"
#include "Common/Globals.h"
#include "Config/ConfigManager.h"
#include "Profiling/Profiling.h"
#include <Windows.h>

namespace Ryu::Engine { class Runtime; }


// WinMain
#define RYU_MAIN()                                 \
int WINAPI wWinMain(                               \
	_In_     MAYBE_UNUSED HINSTANCE hInstance,     \
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance, \
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,     \
	_In_     MAYBE_UNUSED int       nCmdShow)


// Default main entry point
#define RYU_ENTRY_POINT(AppName) RYU_MAIN()                                                           \
{                                                                                                     \
	using namespace Ryu;                                                                              \
	RYU_PROFILE_BOOKMARK("BEGIN RYU");         			                                              \
	Config::ConfigManager::Get().Initialize(RYU_ROOT_DIR RYU_STRING(RYU_CONCAT(/Config, /AppName)));  \
	App::Internal::SetUpDefaultLogger();                                                              \
																			                          \
	Engine::Engine& engine = Engine::Engine::Get();                                                   \
	engine.SetCommandLine(lpCmdLine);                                                                 \
																			                          \
	engine.Run();                                                                                     \
																			                          \
	Config::ConfigManager::Get().SaveAll();                                                           \
	RYU_PROFILE_BOOKMARK("END RYU");                                                                  \
	return 0;                                                                                         \
}

// Define the extern function to create the runtime
#define RYU_USE_APP(RuntimeClass, ...)                        \
std::shared_ptr<::Ryu::App::Application> CreateApplication()  \
{                                                             \
	return std::make_shared<RuntimeClass>(__VA_ARGS__);       \
}

// Define the extern function to create the runtime with main definition
#define RYU_USE_APP_WITH_MAIN(RuntimeClass, ...)              \
std::shared_ptr<::Ryu::App::Application> CreateApplication()  \
{                                                             \
	return std::make_shared<RuntimeClass>(__VA_ARGS__);       \
} RYU_ENTRY_POINT(RuntimeClass)
