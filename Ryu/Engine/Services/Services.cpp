#include "Engine/Services/Services.h"

namespace Ryu::Engine
{
	void ServiceLocator::Initialize()
	{
		if (s_initialized) return;

		s_services.Version = CURRENT_VERSION;
		s_initialized = true;
	}
	
	void ServiceLocator::Shutdown()
	{
		s_services = {};
		s_initialized = false;
	}
	
   /* Services* ServiceLocator::Get() noexcept
    {
        return s_initialized ? &s_services : nullptr;
    }*/

    void ServiceLocator::Register(App::IApplication* app)
    {
		if (s_initialized)
		{
			s_services.App = app;
		}
    }

    void ServiceLocator::Register(Logging::Logger* service)
    {
        if (s_initialized)
        {
            s_services.Logger = service;
        }
    }

    void ServiceLocator::Register(Config::ConfigManager* service)
    {
        if (s_initialized)
        {
            s_services.Config = service;
        }
    }

    void ServiceLocator::Register(Config::CmdLine* service)
    {
        if (s_initialized)
        {
            s_services.CmdLine = service;
        }
    }

    void ServiceLocator::Register(App::PathManager* service)
    {
        if (s_initialized)
        {
            s_services.PathManager = service;
        }
    }

    void ServiceLocator::Register(Gfx::Renderer* service)
    {
        if (s_initialized)
        {
            s_services.Renderer = service;
        }
    }

    void ServiceLocator::Register(Gfx::ShaderCompiler* service)
    {
        if (s_initialized)
        {
            s_services.ShaderCompiler = service;
        }
    }

    void ServiceLocator::Register(Game::InputManager* service)
    {
        if (s_initialized)
        {
            s_services.InputManager = service;
        }
    }
}
