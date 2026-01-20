#include "Game/Core/GameServices.h"

namespace Ryu::Game
{
#define ReturnServiceChecked(Name) \
	Engine::Services* svc = GetEngineServices();\
	return (svc) ? svc->Name : nullptr

	GameServices::GameServices(RyuServices services)
		: m_services(services)
	{
	}
	Logging::Logger* GameServices::GetLogger() const
	{
		ReturnServiceChecked(Logger);
	}

	Config::ConfigManager* GameServices::GetConfigManager() const
	{
		ReturnServiceChecked(Config);
	}
		
	Config::CmdLine* GameServices::GetCmdLine() const
	{
		ReturnServiceChecked(CmdLine);
	}
	
	App::PathManager* GameServices::GetPathManager() const
	{
		ReturnServiceChecked(PathManager);
	}
	
	Gfx::Renderer* GameServices::GetRenderer() const
	{
		ReturnServiceChecked(Renderer);
	}
	
	Gfx::ShaderCompiler* GameServices::GetShaderCompiler() const
	{
		ReturnServiceChecked(ShaderCompiler);
	}
	
	Game::InputManager* GameServices::GetInputManager() const
	{
		ReturnServiceChecked(InputManager);
	}
	
	App::IApplication* GameServices::GetApplication() const
	{
		ReturnServiceChecked(App);
	}
}
