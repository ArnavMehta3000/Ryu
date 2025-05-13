#pragma once
#include "Common/Globals.h"
#include "Logger/Assert.h"
#include "Config/ConfigManager.h"
#include "App/Application.h"
#include "Engine/Engine.h"
#include "Engine/Entry/EngineMain.h"
#include "Profiling/Profiling.h"
#include <filesystem>

namespace fs = std::filesystem;
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
	/**
	 * @brief Simple structure for default 'Main' engine run functionality
	 * @details Abstracts the logic to run the game as DLL and standalone including initial setup and shutdown
	 */
	struct Runner
	{
		// Excluding trailing slash
		fs::path ProjectDir;
#if defined(RYU_GAME_AS_DLL)
		fs::path GameDLLPath;
#endif

		/**
		 * @brief Consider this to be Ryu's main entry point. Use this as reference for custom implementations
		 * @note Always initialize config manager and then the logger otherwise unexpected behavior may occur
		 */
		void Run()
		{
			try
			{
				RYU_PROFILE_BOOKMARK("BEGIN RYU");
				PreInit();

				RYU_ASSERT(!ProjectDir.empty(), "Project directory cannot be empty!");

#if defined(RYU_GAME_AS_DLL)
				RYU_ASSERT(!GameDLLPath.empty(), "Game DLL path cannot be empty if building game as DLL!");
#endif

				RunInternal();
				PostShutdown();
				RYU_PROFILE_BOOKMARK("END RYU");
			}
			catch (const Exception& e)
			{
				using namespace ::Ryu::Logging;

				// Custom version of the RYU_LOG_FATAL macro that includes a stack our assertion exception
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

	private:
		RYU_LOG_DECLARE_CATEGORY(EngineRunner);

		void PreInit()
		{
			// Setup config
			Config::ConfigManager::Get().Initialize((ProjectDir / "Config").string());

			// Setup logger
			App::Internal::SetUpDefaultLogger();
		}

		void PostShutdown()
		{
			// Save configs (if any)
			Config::ConfigManager::Get().SaveAll();
		}

		void RunInternal()
		{
			auto& engine = Engine::Engine::Get();
			engine.m_projectDir = ProjectDir.string();

#if defined(RYU_GAME_AS_DLL)
			const std::string dllPath = GameDLLPath.string();
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(EngineRunner), "Loading engine with game module: {}", dllPath);
			engine.RunWithGameModule(dllPath);
#else
			engine.Run();
#endif
		}
	};
}
