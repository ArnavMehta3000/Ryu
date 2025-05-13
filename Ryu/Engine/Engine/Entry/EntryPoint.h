#pragma once
#include "Logger/Assert.h"
#include "Engine/Engine.h"
#include "App/Application.h"
#include "Common/Globals.h"
#include "Config/ConfigManager.h"
#include "Profiling/Profiling.h"
#include "Engine/Entry/EngineMain.h"


namespace Ryu::Engine { class Runtime; }

#if !defined(RYU_GAME_AS_DLL)
// Define the extern function to create the runtime
// Only enabled when game is not built as a DLL
#define RYU_USE_APP(RuntimeClass, ...)       \
::Ryu::App::Application* CreateApplication() \
{                                            \
	return new RuntimeClass(__VA_ARGS__);    \
}
#else
#define RYU_USE_APP(RuntimeClass, ...)
#endif

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
