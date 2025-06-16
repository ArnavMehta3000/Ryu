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
::Ryu::App::Application* CreateApplication() \
{                                            \
	return new RuntimeClass(__VA_ARGS__);    \
}
#else
#define RYU_USE_APP(RuntimeClass, ...)
#endif

namespace Ryu::Engine
{
	namespace Internal
	{
		static void PreInit()
		{
			// Init path manager to get paths
			auto& pathManager = App::PathManager::Get();
			pathManager.Init();

			// Setup config
			Config::ConfigManager::Get().Initialize((pathManager.GetProjectDir() / "Config").string());
		}

		static void PostShutdown()
		{
			// Save configs (if any)
			Config::ConfigManager::Get().SaveAll();
		}

		static void RunInternal()
		{
			RYU_LOG_DECLARE_CATEGORY(EngineRunner);

			auto& pathManager = App::PathManager::Get();
			auto& engine = Engine::Engine::Get();

			RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(EngineRunner),
				"Using project directory: {}", pathManager.GetProjectDir().string());

			// Run the game as normal or run it with DLL
#if defined(RYU_GAME_AS_DLL)
			const std::string dllPath = pathManager.GetGameDLLPath().string();
			RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(EngineRunner), "Loading engine with game module: {}", dllPath);
			engine.RunWithGameModule(dllPath);
#else
			engine.Run();
#endif
		}
	}



	/**
	* @brief Consider this to be Ryu's main entry point. Use this as reference for custom implementations
	* @note Always initialize config manager and then the logger otherwise unexpected behavior may occur
	*/
	static void Run()
	{
		RYU_LOG_DECLARE_CATEGORY(EngineRunner);
		try
		{
			RYU_PROFILE_BOOKMARK("BEGIN RYU");
			Internal::PreInit();
			Internal::RunInternal();
			Internal::PostShutdown();
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
					.Message = e.what(),
					.Stacktrace = e.GetTrace()
				});
		}
		catch (const std::exception& e)
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(EngineRunner), "Unhandled exception: {}", e.what());
		}
	}
}
