#include "Engine/Engine.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"
#include "Graphics/Core/Core.h"
#include "Memory/New.h"
#include "App/Utils/PathManager.h"
#include "App/AppConfig.h"
#include "Logging/Logger.h"
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

	void PrintMemoryStats()
	{
		using namespace Ryu::Memory;

		RYU_LOG_DEBUG(LogEngine,
			R"(--- Memory Stats (bytes) ---
		Total Allocated: {}
		Current Usage: {}
		Peak Usage: {}
		Allocation: {}
		Deallocations: {}
		Active Allocations: {})",
			GetTotalAllocated(),
			GetCurrentUsage(),
			GetPeakUsage(),
			GetAllocationCount(),
			GetDeallocationCount(),
			(GetAllocationCount() - GetDeallocationCount()));
	}

	bool Engine::Init()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Engine Initialize");

		const App::PathManager& pathManager = m_app->GetPathManager();
		Config::ConfigManager::Get().Initialize((pathManager.GetProjectDir() / "Config").string());

		PrintMemoryStats();

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
		m_renderer->Initialize();

		RYU_PROFILE_BOOKMARK("Initialize script engine");
		m_scriptEngine = std::make_unique<Scripting::ScriptEngine>((
			pathManager.GetProjectDir() / "Scripts").string());

		RYU_LOG_TRACE(LogEngine, "Engine initialization completed");
		return true;
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");
		RYU_LOG_DEBUG(LogEngine, "Shutting down Engine");

		// Flush the log here in case something fails during shutdown
		if (auto* logger = Logging::Internal::GetLoggerInstance())
		{
			logger->Flush();
		}

		m_scriptEngine.reset();
		m_app.reset();

		m_renderer->Shutdown();
		m_renderer.reset();

		Config::ConfigManager::Get().SaveAll();

		RYU_LOG_INFO(LogEngine, "Engine shutdown completed");

		PrintMemoryStats();
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

				Window::Window::ClearPendingEvents();
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

			// Run the app
			MainLoop();

			// Application closing was requested, shut it down
			m_app->OnShutdown();
		}
		catch (const AssertException& e)
		{
			/*
			We will be here only in two cases
			1. Assertion failed
				- Assertion will be handled by the AssertManager handler (setup in Engine/Setup/Setup.cpp)
				- That will re-throw the AssertException and we can catch it here
			2. Fatal Log
				- Fatal log callback will be called (setup in Engine/Setup/Setup.cpp)
				- OnFatal callback will throw AssertException and we can catch it here
			*/

			RYU_LOG_ERROR(LogEngine, "Assertion failed: {}", e.what());
			if (auto* logger = Logging::Internal::GetLoggerInstance())
			{
				logger->Flush();
			}

			std::abort();
		}
		catch (...)
		{
			// No idea what happened, abort
			if (auto* logger = Logging::Internal::GetLoggerInstance())
			{
				logger->Flush();
				spdlog::dump_backtrace();
			}
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
}
