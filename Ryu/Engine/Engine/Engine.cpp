#include "Engine.h"
#include "Logger/Logger.h"
#include "Graphics/GraphicsConfig.h"
#include "Profiling/Profiling.h"

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

		RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Initializing Engine");
		m_app = CreateApplication();

		// Check if debugger is attached
		if (Common::Globals::IsDebuggerAttached())
		{
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "--- A debugger is attached to the Engine!---");
		}

		RYU_PROFILE_BOOKMARK("Initialize runtime application");
		if (InitRuntime())
		{
			RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine application initialized successfully");
		}
		else
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize Engine application");
			return false;
		}
		
		RYU_PROFILE_BOOKMARK("Initialize graphics");
		m_renderer = std::make_unique<Gfx::Renderer>(m_app->GetWindow()->GetHandle());

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine initialized successfully");

		return true;
	}

	bool Engine::InitRuntime()
	{
		RYU_PROFILE_SCOPE();
		
		// Bind resize event before initializing the application
		m_onAppResizedConnection = m_app->GetWindowResizedSignal().Connect(
			[this](const Elos::Event::Resized& e) {OnAppResize(e.Size.Width, e.Size.Height); });

		// Init the application
		return m_app->Init();
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");
		RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Shutting down Engine");

		// Disconnect resized event
		m_onAppResizedConnection.Disconnect();

		m_app->Shutdown();
		m_renderer.reset();

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Shutdown Engine");
	}

	f64 Engine::GetEngineUpTime()
	{
		return m_timer.GetTotalTime();
	}

	void Engine::Run()
	{
		RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(Engine), "Running the Engine");

		if (!Init())
		{
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize Engine! Exiting.");
			return;
		}

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Starting Engine main loop");

		auto appWindow = m_app->GetWindow();
		while (appWindow->IsOpen())
		{
			m_app->ProcessWindowEvents();

			m_timer.Tick([&](const Utils::TimeInfo& info)
			{
				DoFrame(info);
			});
			
			RYU_PROFILE_MARK_FRAME();
		}

		RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Stopping Engine main loop");

		Shutdown();
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

	void Engine::DoFrame(const Utils::TimeInfo& timeInfo)
	{
		RYU_PROFILE_SCOPE();
		
		m_app->Tick(timeInfo);
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
