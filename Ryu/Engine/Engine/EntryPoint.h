#pragma once
#include "Logger/Assert.h"
#include "Engine/Engine.h"
#include "App/Application.h"
#include "Common/Globals.h"
#include "Config/ConfigManager.h"
#include "Profiling/Profiling.h"
#include <Windows.h>

namespace Ryu::Engine { class Runtime; }


#define RYU_DEF_MAIN() int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char *argv[])


// WinMain
#define RYU_WIN_MAIN()                             \
int WINAPI wWinMain(                               \
	_In_     MAYBE_UNUSED HINSTANCE hInstance,     \
	_In_opt_ MAYBE_UNUSED HINSTANCE hPrevInstance, \
	_In_     MAYBE_UNUSED LPWSTR    lpCmdLine,     \
	_In_     MAYBE_UNUSED int       nCmdShow)



#ifdef RYU_BUILD_DEBUG 
#define RYU_MAIN() RYU_DEF_MAIN()
#else
#define RYU_MAIN() RYU_WIN_MAIN()
#endif


// Default main entry point
#define RYU_ENTRY_POINT(AppName) RYU_MAIN()                                                           \
{                                                                                                     \
	using namespace Ryu;                                                                              \
	RYU_PROFILE_BOOKMARK("BEGIN RYU");         			                                              \
	Config::ConfigManager::Get().Initialize(RYU_ROOT_DIR RYU_STRING(RYU_CONCAT(/Config, /AppName)));  \
	App::Internal::SetUpDefaultLogger();                                                              \
																			                          \
	Engine::Engine& engine = Engine::Engine::Get();                                                   \
	engine.Run();                                                                                     \
																			                          \
	Config::ConfigManager::Get().SaveAll();                                                           \
	RYU_PROFILE_BOOKMARK("END RYU");                                                                  \
	return 0;                                                                                         \
}

// Define the extern function to create the runtime
#define RYU_USE_APP(RuntimeClass, ...)                        \
std::unique_ptr<::Ryu::App::Application> CreateApplication()  \
{                                                             \
	return std::make_unique<RuntimeClass>(__VA_ARGS__);       \
}

// Define the extern function to create the runtime with main definition
#define RYU_USE_APP_WITH_MAIN(RuntimeClass, ...)              \
std::unique_ptr<::Ryu::App::Application> CreateApplication()  \
{                                                             \
	return std::make_unique<RuntimeClass>(__VA_ARGS__);       \
} RYU_ENTRY_POINT(RuntimeClass)


namespace Ryu::Engine
{
	struct Runner
	{
		// Excluding trailing slash
		std::string ProjectDir;

		/**
		 * @brief Consider this to be Ryu's main entry point. Use this as reference for custom implementations
		 * @note Always initialize config manager and then the logger otherwise unexpected behavior may occur
		 */
		void Run()
		{
			try
			{
				RYU_PROFILE_BOOKMARK("BEGIN RYU");

				// Setup config
				Config::ConfigManager::Get().Initialize(ProjectDir + "/Config");

				// Setup logger
				App::Internal::SetUpDefaultLogger();

				// Run the engine
				auto& engine = Engine::Engine::Get();
				engine.m_projectDir = ProjectDir;
				engine.Run();

				// Save configs (if any)
				Config::ConfigManager::Get().SaveAll();

				RYU_PROFILE_BOOKMARK("END RYU");
			}
			catch (const Exception& e)
			{
				using namespace ::Ryu::Logging;

				// Custom version of the RYU_LOG_FATAL macro that includes a stack our
				Logger::Get().Log(
					AssertLog, 
					LogLevel::Fatal, 
					LogMessage
					{ 
						.Message    = e.what(),
						.Stacktrace = e.GetTrace() 
					}
				);
			}
		}
	};
}
