#include "Engine/Engine.h"

#include "Application/App/Utils/PathManager.h"
#include "Core/Config/CmdLine.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Globals/Globals.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiling/Profiling.h"
#include "Core/Utils/Timing/Stopwatch.h"
#include "Engine/Services/Services.h"
#include "Game/World/WorldManager.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Math/Math.h"
#include "Memory/New.h"
#include <DirectXMath.h>

namespace Ryu::Engine
{
	static constexpr f64 MAX_STALL_TIME = 1.5;  // Highest ever delta time allowed
	static constexpr f64 FALLBACK_DELTA_TIME = 1.0 / 15.0;  // 15 FPS

	void PrintMemoryStats()
	{
		if (Memory::IsMemoryTrackingEnabled())
		{
			const size_t peakUsage             = Memory::GetPeakUsage();
			const size_t currentUsage          = Memory::GetCurrentUsage();
			const size_t totalAllocated        = Memory::GetTotalAllocated();
			const size_t allocationCount       = Memory::GetAllocationCount();
			const size_t deallocationCount     = Memory::GetDeallocationCount();
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

	bool Engine::Init(Gfx::IRendererHook* rendererHook)
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Engine Initialize");

		ServiceLocator::Initialize();
		Config::ConfigManager::Get().Initialize(App::PathManager::Get().GetConfigDir());
		

		RYU_LOG_DEBUG("Initializing Engine");

		if (!DirectX::XMVerifyCPUSupport())
		{
			RYU_LOG_FATAL("DirectX is not supported on this system");
			return false;
		}

		if (Globals::IsDebuggerAttached())
		{
			RYU_LOG_INFO("--- A debugger is attached to the Engine!---");
		}

		RYU_PROFILE_BOOKMARK("Initialize graphics");

		// Get window from current application
		Window::Window* window = m_currentApp->GetWindow();
		RYU_ASSERT(window != nullptr, "Application must have a valid window");

		m_renderer = std::make_unique<Gfx::Renderer>(window->GetHandle(), rendererHook);

		// Init input manager
		m_inputManager = std::make_unique<Game::InputManager>(
			window->GetInput(),
			window->GetDispatcher());

		// Register services
		ServiceLocator::Register(m_currentApp);
		ServiceLocator::Register(&Logging::Logger::Get());
		ServiceLocator::Register(&Config::ConfigManager::Get());
		ServiceLocator::Register(&Config::CmdLine::Get());
		ServiceLocator::Register(&App::PathManager::Get());
		ServiceLocator::Register(m_renderer.get());
		ServiceLocator::Register(m_inputManager.get());
		ServiceLocator::Register(&Gfx::ShaderCompiler::Get());

		RYU_LOG_TRACE("Engine initialization completed");
		return true;
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");

		// Unsubscribe listeners
		if (m_currentApp)
		{
			if (Window::Window* window = m_currentApp->GetWindow())
			{
				window->Unsubscribe(m_resizeListener);
				window->Unsubscribe(m_closeListener);
			}
		}

		m_inputManager.reset();
		m_renderer.reset();

		m_currentApp = nullptr;

		Config::ConfigManager::Get().SaveAll();

		PrintMemoryStats();
	}

	void Engine::MainLoop()
	{
		Utils::FrameTimer frameTimer;
		frameTimer.SetMaxDeltaTime(FALLBACK_DELTA_TIME);

		RYU_ASSERT(m_currentApp, "No application to run!");
		if (!m_currentApp) 
		{
			RYU_LOG_ERROR("No application to run!");
			return;
		}

		Window::Window* appWindow = m_currentApp->GetWindow();
		if (!appWindow)
		{
			RYU_LOG_ERROR("Application has no window!");
			return;
		}

		while (m_currentApp->IsRunning())
		{
			frameTimer.Tick();

			appWindow->Update();
			m_inputManager->Update();
			m_currentApp->OnTick(frameTimer);

			// Render world if present
			if (Game::WorldManager* manager = m_currentApp->GetWorldManager()) [[likely]]
			{
				if (Game::World* world = manager->GetActiveWorld()) [[likely]]
				{
					m_renderer->RenderWorld(*world, frameTimer);
				}
			}

			appWindow->ProcessEventQueue();

			RYU_PROFILE_MARK_FRAME();
		}
	}

	void RYU_API Engine::RunApp(App::IApplication* app, Gfx::IRendererHook* rendererHook)
	{
		using namespace Ryu::Logging;

		RYU_ASSERT(app != nullptr, "Application cannot be nullptr");
		Logging::Logger& logger = Logging::Logger::Get();

		m_currentApp = app;

		// Init engine
		if (!Init(rendererHook))
		{
			RYU_LOG_FATAL("Failed to initialize Engine! Exiting.");
			return;
		}

		// Create window event listeners
		Window::Window* window = m_currentApp->GetWindow();
		RYU_ASSERT(window != nullptr, "Application window cannot be null");

		m_resizeListener = window->Subscribe<Window::ResizeEvent>(
		[this](const Window::ResizeEvent& e)
		{
			OnAppResize(e.Width, e.Height);
		});

		m_closeListener = window->Subscribe<Window::CloseEvent>(
		[this](const Window::CloseEvent&)
		{
			Quit();
		});

		// ----- Main loop ----- 
		try
		{
			// Init & run app
			if (m_currentApp->OnInit()) [[likely]]
			{
				MainLoop();
			}
			else
			{
				RYU_LOG_FATAL("Failed to initialize application! Exiting.");
			}

			// Application closing was requested, shut it down (could be editor)
			m_currentApp->OnShutdown();
		}
		catch (const AssertException& e)
		{
			RYU_LOG_ERROR("Assertion failed: {}", e.what());
			logger.Flush();
			std::abort();
		}
		catch (...)
		{
			logger.Flush();
			spdlog::dump_backtrace();
			
			std::abort();
		}

		// Shutdown engine
		Shutdown();
	}

	void Engine::RunApp(std::shared_ptr<App::App> app, Gfx::IRendererHook* rendererHook)
	{
		RunApp(static_cast<App::IApplication*>(app.get()), rendererHook);
	}

	void Engine::Quit() const noexcept
	{
		if (m_currentApp)
		{
			RYU_PROFILE_BOOKMARK("Requesting application shutdown");
			m_currentApp->RequestQuit();
		}
	}

	void Engine::OnAppResize(u32 width, u32 height) const noexcept
	{
		// Flush the log here in case something fails while resizing the renderer
		Logging::Logger::Get().Flush();

		if (m_renderer)
		{
			m_renderer->OnResize(width, height);
		}
	}
}
