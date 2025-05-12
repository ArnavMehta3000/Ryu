#include "Engine/Engine.h"
#include "Engine/EntryPoint.h"
#include "Testbench/TestApp.h"
#include <filesystem>

namespace fs = std::filesystem;

/*
* This script/project demos how to run the game project as a DLL
*	- Build with --ryu-game-as-dll=y which will make RyuTestbench be built as a dll
*	- The runner is an example main that shows how to load the DLL
*	- This is how the editor will load the game module
*/

RYU_MAIN()
{
	RYU_LOG_DECLARE_CATEGORY(GameRunner);

	const std::string projectDir = RYU_ROOT_DIR "/Testbench";
	const std::string gameDllPath = (std::filesystem::current_path() / "RyuTestbench.dll").string();
	
	Ryu::Config::ConfigManager::Get().Initialize(projectDir + "/Config");
	Ryu::App::Internal::SetUpDefaultLogger();

	RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(GameRunner), "Loading game module: {}", gameDllPath);

	auto& engine = Ryu::Engine::Engine::Get();
	engine.m_projectDir = projectDir;
	engine.RunWithGameModule(gameDllPath);

	RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(GameRunner), "Game Runner shutting down");
	return 0;
}
