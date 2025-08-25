#include "Engine/Engine.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"
#include "Graphics/Core/Core.h"
#include "Memory/New.h"
#include "Math/Math.h"
#include "App/Utils/PathManager.h"
#include "App/AppConfig.h"
#include "Logging/Logger.h"
#include "Profiling/Profiling.h"
#include <DirectXMath.h>
#include <wrl/event.h>

namespace Ryu::Engine
{
	void PrintMemoryStats()
	{
		if (Memory::IsMemoryTrackingEnabled())
		{
			const size_t totalAllocated = Memory::GetTotalAllocated();
			const size_t currentUsage = Memory::GetCurrentUsage();
			const size_t peakUsage = Memory::GetPeakUsage();
			const size_t allocationCount = Memory::GetAllocationCount();
			const size_t deallocationCount = Memory::GetDeallocationCount();
			const size_t activeAllocationCount = (allocationCount - deallocationCount);

			RYU_LOG_DEBUG(R"(--- Memory Stats ---
				  Total Allocated: {} ({:.2f}MB)
				  Current Usage: {} ({:.2f}MB)
				  Peak Usage: {} ({:.2f}MB)
				  Allocations Count: {}
				  Deallocations Count: {}
				  Active Allocations Count: {})",
				totalAllocated, Math::BytesToMB(totalAllocated),
				currentUsage, Math::BytesToMB(currentUsage),
				peakUsage, Math::BytesToMB(currentUsage),
				allocationCount,
				deallocationCount,
				activeAllocationCount);
		}
		else
		{
			RYU_LOG_WARN("Tried to report memory stats, but memory tracking is disabled! See Memory/New.cpp");
		}
	}

	bool Engine::Init()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Engine Initialize");

		// Setup path manager
		const App::PathManager& pathManager = m_app->GetPathManager();
		Config::ConfigManager::Get().Initialize((pathManager.GetProjectDir() / "Config").string());

		RYU_LOG_DEBUG("Initializing Engine");

		if (!DirectX::XMVerifyCPUSupport())
		{
			RYU_LOG_FATAL("DirectX is not supported on this system");
			return false;
		}

		// Check if debugger is attached
		if (Globals::IsDebuggerAttached())
		{
			RYU_LOG_INFO("--- A debugger is attached to the Engine!---");
		}

		RYU_PROFILE_BOOKMARK("Initialize graphics");
		Gfx::Core::Init(m_app->GetWindow()->GetHandle());

		RYU_PROFILE_BOOKMARK("Initialize script engine");
		m_scriptEngine = std::make_unique<Scripting::ScriptEngine>((
			pathManager.GetProjectDir() / "Scripts").string());

		RYU_LOG_TRACE("Engine initialization completed");
		return true;
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");
		RYU_LOG_DEBUG("Shutting down Engine");

		// Flush the log here in case something fails during shutdown
		if (auto* logger = Logging::Internal::GetLoggerInstance())
		{
			logger->Flush();
		}
		
		// Unsubscribe listeners
		auto window = m_app->GetWindow();
		window->Unsubscribe(m_resizeListener);
		window->Unsubscribe(m_closeListener);

		m_scriptEngine.reset();
		m_app.reset();

		Gfx::Core::Shutdown();

		Config::ConfigManager::Get().SaveAll();

		PrintMemoryStats();

		RYU_LOG_INFO("Engine shutdown completed");

		// Shutdown the logger
		if (auto* logger = Logging::Internal::GetLoggerInstance())
		{
			logger->Flush();
			logger->Shutdown();
		}
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
				
				Gfx::Core::Render();

				m_app->GetWindow()->ProcessEventQueue();
			}
		}
		else
		{
			RYU_LOG_FATAL("Failed to initialize application! Exiting.");
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
			RYU_LOG_FATAL("Failed to initialize Engine! Exiting.");
			return;
		}

		// Create window event listeners
		auto window = m_app->GetWindow();

		m_resizeListener = window->On<Window::ResizeEvent>([this] (const Window::ResizeEvent& e)
		{
			OnAppResize(e.Width, e.Height);
		});

		m_closeListener = window->On<Window::CloseEvent>([this] (const Window::CloseEvent&)
		{
			Quit(); 
		});

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

			RYU_LOG_ERROR("Assertion failed: {}", e.what());
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
		RYU_LOG_TRACE("Engine::OnAppResize -  {}x{}", width, height);

		if (auto* logger = Logging::Internal::GetLoggerInstance())
		{
			// Flush the log here in case something fails
			logger->Flush();
		}

		Gfx::Core::Resize(width, height);
	}
}
