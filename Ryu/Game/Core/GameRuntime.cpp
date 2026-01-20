#include "Game/Core/GameRuntime.h"

#include "Application/App/Utils/PathManager.h"
#include "Core/Config/CmdLine.h"
#include "Core/Config/ConfigManager.h"
#include "Engine/Engine.h"
#include "Engine/Services/Services.h"
#include "Graphics/Compiler/ShaderCompiler.h"

namespace Ryu::Game
{
	App::IApplication* Runtime::GetApplication()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetApplication() : nullptr;
#else
		return Engine::Engine::Get().GetApplication();
#endif
	}

	Logging::Logger* Runtime::GetLogger()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetLogger() : nullptr;
#else
		return &Logging::Logger::Get();
#endif
	}
	
	Config::ConfigManager* Runtime::GetConfigManager()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetConfigManager() : nullptr;
#else
		return &Config::ConfigManager::Get();
#endif
	}
	
	Config::CmdLine* Runtime::GetCmdLine()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetCmdLine() : nullptr;
#else
		return &Config::CmdLine::Get();
#endif
	}
	
	App::PathManager* Runtime::GetPathManager()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetPathManager() : nullptr;
#else
		return &App::PathManager::Get();
#endif
	}
	Gfx::Renderer* Runtime::GetRenderer()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetRenderer() : nullptr;
#else
		return Engine::Engine::Get().GetRenderer();
#endif
	}
	Gfx::ShaderCompiler* Runtime::GetShaderCompiler()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetShaderCompiler() : nullptr;
#else
		return &Gfx::ShaderCompiler::Get();
#endif
	}
	
	Game::InputManager* Runtime::GetInputManager()
	{
#if defined(RYU_HOT_RELOAD)
		return (Internal::g_services) ? Internal::g_services->GetInputManager() : nullptr;
#else
		return Engine::Engine::Get().GetInputManager();
#endif
	}
}
