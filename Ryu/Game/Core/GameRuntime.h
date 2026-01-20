#pragma once
#include "Game/Core/GameServices.h"

namespace Ryu::Game
{
	class Runtime
	{
	public:
		[[nodiscard]] static App::IApplication* GetApplication();
		[[nodiscard]] static Logging::Logger* GetLogger();
		[[nodiscard]] static Config::ConfigManager* GetConfigManager();
		[[nodiscard]] static Config::CmdLine* GetCmdLine();
		[[nodiscard]] static App::PathManager* GetPathManager();
		[[nodiscard]] static Gfx::Renderer* GetRenderer();
		[[nodiscard]] static Gfx::ShaderCompiler* GetShaderCompiler();
		[[nodiscard]] static Game::InputManager* GetInputManager();
	};
}

#define RYU_RUNTIME() ::Ryu::Game::Runtime
