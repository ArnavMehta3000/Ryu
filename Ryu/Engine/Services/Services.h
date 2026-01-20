#pragma once
#include "Engine/Services/ServiceTypes.h"

namespace Ryu::Engine
{
	struct Services
	{
		App::IApplication*     App            = nullptr;
		Logging::Logger*       Logger         = nullptr;
		Config::ConfigManager* Config         = nullptr;
		Config::CmdLine*       CmdLine        = nullptr;
		App::PathManager*      PathManager    = nullptr;
		Gfx::Renderer*         Renderer       = nullptr;
		Gfx::ShaderCompiler*   ShaderCompiler = nullptr;
		Game::InputManager*    InputManager   = nullptr;
		EngineVersion          Version;
	};

	class ServiceLocator
	{
	public:
		static void Initialize();
		static void Shutdown();

		inline static Services* Get() { return s_initialized ? &s_services : nullptr; }

		static void Register(App::IApplication* app);
		static void Register(Logging::Logger* logger);
		static void Register(Config::ConfigManager* config);
		static void Register(Config::CmdLine* cmdLine);
		static void Register(App::PathManager* pathManager);
		static void Register(Gfx::Renderer* renderer);
		static void Register(Gfx::ShaderCompiler* shaderCompiler);
		static void Register(Game::InputManager* inputManager);

	private:
		static inline Services s_services{};
		static inline bool s_initialized = false;
	};
}
