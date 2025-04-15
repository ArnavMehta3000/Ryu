#include "Engine.h"
#include "Logger/Logger.h"
#include "GraphicsRHI/GraphicsConfig.h"
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

		LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Initializing Engine");
		m_app = CreateApplication();

		// Check if debugger is attached
		if (Common::Globals::IsDebuggerAttached())
		{
			LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "--- A debugger is attached to the Engine!---");
		}

		RYU_PROFILE_BOOKMARK("Initialize runtime application");
		if (InitRuntime())
		{
			LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine application initialized successfully");
		}
		else
		{
			LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize Engine application");
			return false;
		}
		

		RYU_PROFILE_BOOKMARK("Initialize graphics");
		m_renderer = std::make_unique<Graphics::Renderer>();
		if (VoidResult result = Graphics::InitGraphics(m_renderer.get(), m_app->GetWindow()->GetHandle()); !result)
		{
			LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize Graphics. Error: {}", result.error());
		}
		else
		{
			LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Graphics ({}) initialized successfully", EnumToString(Graphics::GraphicsConfig::Get().GraphicsAPI.Get()));
		}

		//m_renderSurface.Window = m_app->GetWindow().get();
		//m_renderSurface.Surface = Graphics::CreateSurface(m_renderSurface.Window);

		//DEBUG_ASSERT(m_renderSurface.Surface != nullptr, "Failed to create render surface");


		LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine initialized successfully");

		return true;
	}

	bool Engine::InitRuntime()
	{
		RYU_PROFILE_SCOPE();
		// Bind the events before initializing the application
		/*m_app->GetWindowResizedSignal().Connect(([this](const App::Events::OnWindowResize& event)
		{
			OnAppResize(event.Width, event.Height);
		});*/

		// Init the application
		return m_app->Init();
	}

	void Engine::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		RYU_PROFILE_BOOKMARK("Begin Shutdown");
		LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Shutting down Engine");

		m_app->Shutdown();
		Graphics::ShutdownGraphics(m_renderer.get());

		LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Shutdown Engine");
	}

	void Engine::SetCommandLine(std::wstring_view cmdLine)
	{
		m_cmdLine = Config::CommandLine(cmdLine);
	}

	f64 Engine::GetEngineUpTime()
	{
		return m_timer.GetTotalTime();
	}

	void Engine::Run()
	{
		LOG_DEBUG(RYU_LOG_USE_CATEGORY(Engine), "Running the Engine");

		if (!Init())
		{
			LOG_FATAL(RYU_LOG_USE_CATEGORY(Engine), "Failed to initialize Engine! Exiting.");
			return;
		}

		LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Starting Engine main loop");

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

		LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Stopping Engine main loop");

		Shutdown();
	}

	void Engine::Quit() const noexcept
	{
		if (m_app)
		{
			RYU_PROFILE_BOOKMARK("Requesting application shutdown");
			LOG_INFO(RYU_LOG_USE_CATEGORY(Engine), "Rquesting application shutdown");
			// Send a message to the application window to close
			::SendMessage(m_app->GetWindow()->GetHandle(), WM_CLOSE, 0, 0);
		}
	}

	void Engine::DoFrame(const Utils::TimeInfo& timeInfo)
	{
		RYU_PROFILE_SCOPE();
		
		m_app->Tick(timeInfo);
		m_renderer->BeginFrame();
		m_renderer->EndFrame();
	}

	void Engine::OnAppResize(u32 width, u32 height) const noexcept
	{
		LOG_TRACE(RYU_LOG_USE_CATEGORY(Engine), "Engine::OnAppResize -  {}x{}", width, height);

		//if (Graphics::IsInitialized())
		//{
		//	Graphics::ResizeSurface(width, height);
		//}
	}
}
