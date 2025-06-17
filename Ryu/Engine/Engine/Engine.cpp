#include "Engine/Engine.h"
#include "App/Utils/PathManager.h"
#include "App/AppConfig.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Logger/Sinks/FileSink.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"
#include "Utils/MessageBox.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"
#include <DirectXMath.h>
#include <wrl/event.h>

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

		const App::PathManager& pathManager = m_app->GetPathManager();
		Config::ConfigManager::Get().Initialize((pathManager.GetProjectDir() / "Config").string());

		SetupLogger();

		RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(Engine), "Initializing Engine");

		if (!DirectX::XMVerifyCPUSupport())
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "DirectX is not supported on this system");
			return false;
		}

		// Check if debugger is attached
		if (Common::Globals::IsDebuggerAttached())
		{
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "--- A debugger is attached to the Engine!---");
		}
		
		// Load configs
		RYU_PROFILE_BOOKMARK("Initialize graphics");
		m_renderer = std::make_unique<Gfx::Renderer>(m_app->GetWindow()->GetHandle());
		

		RYU_PROFILE_BOOKMARK("Initialize script engine");
		m_scriptEngine = std::make_unique<Scripting::ScriptEngine>((
			pathManager.GetProjectDir() / "Scripts").string());

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine initialization completed");

		return true;
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");
		RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(Engine), "Shutting down Engine");

		m_scriptEngine.reset();
		m_app.reset();
		
#if defined(RYU_GAME_AS_DLL)
		UnloadGameModule();
#endif
		m_renderer.reset();
		
		Config::ConfigManager::Get().SaveAll();

		RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Engine shutdown completed");
	}

	void Engine::MainLoop()
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

				if (m_renderer)
				{
					m_renderer->Render();
				}
			}
		}
		else
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize application! Exiting.");
		}
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
			m_app->m_isRunning = false;
		}
	}

	void Engine::RunApp(std::shared_ptr<App::App> app)
	{
		RYU_ASSERT(app, "Application cannot be nullptr");

		Microsoft::WRL::Wrappers::RoInitializeWrapper InitializeWinRT(RO_INIT_MULTITHREADED);
		m_app = app;

		// Init engine
		if (!Init())
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize Engine! Exiting.");
			return;
		}

		// Close engine on application exit
		auto visitor = Window::WindowEventVisitor
		{
			[this](const Window::CloseEvent&)  // Handle window close event
			{
				if (m_app)
				{
					RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(Engine), "Application window closed, shutting down...");
					m_app->m_isRunning = false;
				}
			},
			[this](const Window::ResizeEvent& e)  // Handle window resize event
			{
				OnAppResize(e.Width, e.Height);
			}
		};
		m_app->GetWindow()->AddEventListener([&visitor](const Window::WindowEvent& e) { std::visit(visitor, e); });

		// Main loop
		try
		{
			MainLoop();
			m_app->OnShutdown();
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

	void Engine::OnAppResize(u32 width, u32 height) const noexcept
	{
		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine::OnAppResize -  {}x{}", width, height);
		if (m_renderer)
		{
			m_renderer->OnResize(width, height);
		}
	}

	void Engine::SetupLogger()
	{
		using namespace Ryu::App;
		using namespace Ryu::Logging;
		using namespace Ryu::Utils;
		using namespace Ryu::Common;

		Logger& logger = Logger::Get();
		const AppConfig& config = AppConfig::Get();

		logger.SetOnFatalCallback([](LogLevel level, const LogMessage& message)
		{
			Utils::MessageBoxDesc desc;
			desc.Title        = EnumToString(level);
			desc.Title       += " Error";
			desc.Text         = message.Message;
			desc.Flags.Button = Utils::MessagBoxButton::Ok;
			desc.Flags.Icon   = Utils::MessageBoxIcon::Error;

			Utils::ShowMessageBox(desc);
			std::abort();
		});

		// Log to output window only when debugger is attached
		if (Globals::IsDebuggerAttached() || config.ForceLogToOutput)
		{
			logger.AddSink(std::make_unique<Logging::DebugSink>());
		}

		if (config.EnableLogToConsole)
		{
			logger.AddSink(std::make_unique<Logging::ConsoleSink>());
		}

		if (config.EnableLogToFile)
		{
			logger.AddSink(std::make_unique<Logging::FileSink>(config.LogFilePath.Get()));
			RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Application log file opened: {}", config.LogFilePath.Get());
		}

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Logger initialized");
	}
}
