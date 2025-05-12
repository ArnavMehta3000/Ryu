#include "Engine/Engine.h"
#include "Engine/EntryPoint.h"
#include "Testbench/Test.h"
#include <filesystem>

namespace fs = std::filesystem;

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
