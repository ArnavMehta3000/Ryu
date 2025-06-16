#include "Engine/Engine.h"
#include "App/Utils/PathManager.h"
#include "Logger/Assert.h"
#include "Logger/Logger.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include <DirectXMath.h>
#include <wrl/event.h>

#if !defined(RYU_GAME_AS_DLL)
#endif

#pragma comment(lib, "runtimeobject.lib")

namespace Ryu::Engine
{
	Engine::Engine()
		: m_app(nullptr)
	{
	}

	bool Engine::Init()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Engine Initialize");

		if (!DirectX::XMVerifyCPUSupport())
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "DirectX is not supported on this system");
			return false;
		}

		RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Initializing Engine");

//#if !defined(RYU_GAME_AS_DLL)  // If the game is not compiled as a DLL then create the application
//		m_app = std::move(std::unique_ptr<App::Application>(CreateApplication()));
//#endif

		// Check if debugger is attached
		if (Common::Globals::IsDebuggerAttached())
		{
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "--- A debugger is attached to the Engine!---");
		}

		if (m_app && m_app->GetWindow())
		{
			RYU_PROFILE_BOOKMARK("Initialize graphics");
			m_renderer = std::make_unique<Gfx::Renderer>(m_app->GetWindow()->GetHandle());
		}

		RYU_PROFILE_BOOKMARK("Initialize script engine");
		m_scriptEngine = std::make_unique<Scripting::ScriptEngine>((
			App::PathManager::Get().GetProjectDir() / "Scripts").string());

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine initialized successfully");

		return true;
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");
		RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Shutting down Engine");

		m_scriptEngine.reset();
		m_app.reset();
		
#if defined(RYU_GAME_AS_DLL)
		UnloadGameModule();
#endif
		m_renderer.reset();

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Shutdown Engine");
	}

	f64 Engine::GetEngineUpTime()
	{
		return m_timer.GetTotalTime();
	}

	void Engine::Quit() const noexcept
	{
		if (m_app)
		{
			RYU_PROFILE_BOOKMARK("Requesting application shutdown");
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Rquesting application shutdown");
			// Send a message to the application window to close
			::SendMessage(m_app->GetWindow()->GetHandle(), WM_CLOSE, 0, 0);
		}
	}

	void Engine::RunApp(std::shared_ptr<App::App> app)
	{
		RYU_ASSERT(app, "Application cannot be nullptr");

		Microsoft::WRL::Wrappers::RoInitializeWrapper InitializeWinRT(RO_INIT_MULTITHREADED);
		m_app = app;
		m_app->PreInit();  // todo maybe move logic to engine

		// Init engine
		if (!Init())
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize Engine! Exiting.");
			return;
		}

		// TODO: Hookup window events (eg. resize for renderer)
		try
		{
			m_app->m_isRunning = m_app->OnInit();
			if (m_app->IsRunning())
			{
				while (m_app->IsRunning())
				{
					m_timer.Tick([this](const Utils::TimeInfo& info)
					{
						m_app->ProcessWindowEvents();
						m_app->OnTick(info);
					});
				}
			}
			else
			{
				RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize application! Exiting.");
			}
			m_app->OnShutdown();
			m_app->PostShutdown();
		}
		catch (const Exception& e)
		{
			using namespace Ryu::Logging;
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
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Unhandled exception: {}", e.what());
		}

		// Shutdown engine
		Shutdown();
	}

	void RYU_API Engine::RunWithGameModule(MAYBE_UNUSED const std::string& gameDllPath)
	{
#if defined(RYU_GAME_AS_DLL)
		if (!LoadGameModule(gameDllPath))
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to load game module: {}", gameDllPath);
			return;
		}

		m_app = std::move(std::unique_ptr<App::Application>(m_gameModuleLoader->GetApplication()));

		Run();
#endif
	}

	bool RYU_API Engine::LoadGameModule(MAYBE_UNUSED const std::string& gameDllPath)
	{
#if defined(RYU_GAME_AS_DLL)
		m_gameModuleLoader = std::make_unique<GameModuleLoader>();
		return m_gameModuleLoader->LoadModule(gameDllPath);
#else
		return false;  // Sould never get here
#endif
	}

	void RYU_API Engine::UnloadGameModule()
	{
#if defined(RYU_GAME_AS_DLL)
		if (m_gameModuleLoader)
		{
			m_gameModuleLoader->UnloadModule();
			m_gameModuleLoader.reset();
		}
#endif
	}

	bool RYU_API Engine::IsGameModuleLoaded() const
	{
#if defined(RYU_GAME_AS_DLL)
		return m_gameModuleLoader && m_gameModuleLoader->IsModuleLoaded();
#else
		return true;
#endif
	}

	void Engine::OnAppResize(u32 width, u32 height) const noexcept
	{
		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine::OnAppResize -  {}x{}", width, height);
		if (m_renderer)
		{
			m_renderer->OnResize(width, height);
		}
	}
}
