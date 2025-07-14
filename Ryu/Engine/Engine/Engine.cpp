#include "Engine/Engine.h"
#include "Globals/Globals.h"
#include "App/Utils/PathManager.h"
#include "App/AppConfig.h"
#include "Logger/Sinks/DebugSink.h"
#include "Logger/Sinks/ConsoleSink.h"
#include "Logger/Sinks/FileSink.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"
#include "Utils/MessageBox.h"
#include "Profiling/Profiling.h"
#include <DirectXMath.h>
#include <wrl/event.h>

namespace Ryu::Engine
{
	namespace
	{
		// Logger pointer used by plugins
		Logging::Logger* g_pluginLogger = nullptr;
	}

	RYU_LOG_DECLARE_CATEGORY(Engine);

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

		RYU_LOG_DEBUG(LogEngine, "Initializing Engine");

		if (!DirectX::XMVerifyCPUSupport())
		{
			RYU_LOG_FATAL(LogEngine, "DirectX is not supported on this system");
			return false;
		}

		// Check if debugger is attached
		if (Globals::IsDebuggerAttached())
		{
			RYU_LOG_INFO(LogEngine, "--- A debugger is attached to the Engine!---");
		}

		RYU_PROFILE_BOOKMARK("Initialize graphics");
		m_renderer = std::make_unique<Gfx::Renderer>(m_app->GetWindow()->GetHandle());

		RYU_PROFILE_BOOKMARK("Initialize script engine");
		m_scriptEngine = std::make_unique<Scripting::ScriptEngine>((
			pathManager.GetProjectDir() / "Scripts").string());

		RYU_LOG_TRACE(LogEngine, "Engine initialization completed");

		// Setup plugin context and load them
		RYU_PROFILE_BOOKMARK("Load plguins");
		m_engineContext.GetLogger   = []     { return g_pluginLogger;   };
		m_engineContext.GetApp      = [this] { return m_app.get();      };
		m_engineContext.GetRenderer = [this] { return m_renderer.get(); };

		InitializePlugins(Plugin::PluginPhase::OnLoad);

		return true;
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");
		RYU_LOG_DEBUG(LogEngine, "Shutting down Engine");

		ShutdownPlugins(Plugin::PluginPhase::OnUnload);

		m_scriptEngine.reset();
		m_app.reset();
		m_renderer.reset();

		Config::ConfigManager::Get().SaveAll();

		RYU_LOG_INFO(LogEngine, "Engine shutdown completed");
	}

	void Engine::MainLoop()
	{
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
			RYU_LOG_FATAL(LogEngine, "Failed to initialize application! Exiting.");
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
		using namespace Ryu::Logging;
		using namespace Microsoft::WRL::Wrappers;

		RYU_ASSERT(app, "Application cannot be nullptr");

		RoInitializeWrapper InitializeWinRT(RO_INIT_MULTITHREADED);
		m_app = app;

		// Init engine
		if (!Init())
		{
			RYU_LOG_FATAL(LogEngine, "Failed to initialize Engine! Exiting.");
			return;
		}

		// Close engine on application exit
		auto visitor = Window::WindowEventVisitor
		{
			[this](const Window::CloseEvent&)  // Handle window close event
			{
				if (m_app)
				{
					RYU_LOG_DEBUG(LogEngine, "Application window closed, shutting down...");
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
			// Init app
			m_app->m_isRunning = m_app->OnInit();

			// Plugin init callback
			InitializePlugins(Plugin::PluginPhase::OnInitialize);

			// Run the app
			MainLoop();

			// Plugin shutdown callback
			ShutdownPlugins(Plugin::PluginPhase::OnShutdown);

			// Application closing was requested, shut it down
			m_app->OnShutdown();
		}
		catch (const Exception& e)
		{
			// Custom version of the RYU_LOG_FATAL macro that includes a stack our assertion exception
			Logging::Internal::InvokeLogger(
				LogEngine,
				LogLevel::Fatal,
				LogMessage
				{
					.Message = e.what(),
					.Stacktrace = e.GetTrace()
				});
		}
		catch (...)
		{
			std::abort();
		}

		// Shutdown engine
		Shutdown();
	}

	void Engine::OnAppResize(u32 width, u32 height) const noexcept
	{
		RYU_LOG_TRACE(LogEngine, "Engine::OnAppResize -  {}x{}", width, height);
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


		if (auto logger = Globals::GetServiceLocator().GetService<Logger>().value_or(nullptr))
		{
			g_pluginLogger = logger;
			const AppConfig& config = AppConfig::Get();

			logger->SetOnFatalCallback([](LogLevel level, const LogMessage& message)
			{
				Utils::MessageBoxDesc desc;
				desc.Title = EnumToString(level);
				desc.Title += " Error";
				desc.Text = message.Message;
				desc.Flags.Button = Utils::MessagBoxButton::Ok;
				desc.Flags.Icon = Utils::MessageBoxIcon::Error;

				Utils::ShowMessageBox(desc);
				std::abort();
			});

			// Log to output window only when debugger is attached
			if (Globals::IsDebuggerAttached() || config.ForceLogToOutput)
			{
				logger->AddSink(std::make_unique<Logging::DebugSink>());
			}

			if (config.EnableLogToConsole)
			{
				logger->AddSink(std::make_unique<Logging::ConsoleSink>());
			}

			if (config.EnableLogToFile)
			{
				logger->AddSink(std::make_unique<Logging::FileSink>(config.LogFilePath.Get()));
				RYU_LOG_TRACE(LogEngine, "Application log file opened: {}", config.LogFilePath.Get());
			}

			RYU_LOG_TRACE(LogEngine, "Logger initialized");
		}
		else
		{
			throw std::runtime_error("Failed to initialize logger");
		}
	}

	void Engine::InitializePlugins(Plugin::PluginPhase phase)
	{
		auto& enginePlugins = App::AppConfig::Get().EnginePlugins.Get();

		for (auto& name : enginePlugins)
		{
			if (auto result = m_pluginManager.LoadPluginInterface<EnginePluginInterface>(name); result)
			{
				result.value().Initialize(phase, &m_engineContext);
			}
			else
			{
				RYU_LOG_ERROR(LogEngine, "Failed to load/init plugin: {}", name);
			}
		}
	}

	void Engine::ShutdownPlugins(Plugin::PluginPhase phase)
	{
		auto& enginePlugins = App::AppConfig::Get().EnginePlugins.Get();

		for (auto& name : enginePlugins)
		{
			if (auto result = m_pluginManager.LoadPluginInterface<EnginePluginInterface>(name); result)
			{
				result.value().Shutdown(phase);
			}
			else
			{
				RYU_LOG_ERROR(LogEngine, "Failed to unload/shotdown plugin: {}", name);
			}
		}
	}
}
