#include "Engine.h"
#include "App/Application.h"
#include "App/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/Config.h"
#include <External/StepTimer/StepTimer.h>

namespace Ryu::Engine
{
	RYU_SINGLETON_IMPL(Engine);

	namespace 
	{
		DX::StepTimer s_timer;

		// Register all the required singletons
		void InitSingletons()
		{
			Utils::SingletonRegistry::Register<Logging::Logger>();
		}
	}

	Engine::Engine()
		: m_app(nullptr)
	{
		InitSingletons();
	}

	Engine::~Engine() = default;

	bool Engine::Init()
	{
		LOG_INFO(RYU_USE_LOG_CATEGORY(Engine), "Initializing Engine");

		// Check if debugger is attached
		if (::IsDebuggerPresent())
		{
			LOG_INFO(RYU_USE_LOG_CATEGORY(Engine), "--- A debugger is attached to the Engine!---");
		}

		// Initialize the application
		if (InitApplication())
		{
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine application initialized successfully");
		}
		else
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Failed to initialize Engine application");
			return false;
		}
		
		Graphics::API api = Graphics::GraphicsConfig::Get().GraphicsAPI;
		if (Ryu::Graphics::Init(api))
		{
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Graphics ({}) initialized successfully", EnumToString(api));
		}
		else
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Failed to initialize Graphics");
			return false;
		}

		m_renderSurface.Window = m_app->GetWindow().get();
		m_renderSurface.Surface = Graphics::CreateSurface(m_renderSurface.Window);

		ASSERT(m_renderSurface.Surface != nullptr, "Failed to create render surface");

		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine initialized successfully");

		return true;
	}

	bool Engine::InitApplication()
	{
		// Bind the events before initializing the application
		std::ignore = m_app->OnWindowResize.Add([this](const App::Events::OnWindowResize& event)
		{
			OnAppResize(event.Width, event.Height);
		});

		// Init the application
		return m_app->Init();
	}

	void Engine::Shutdown()
	{
		LOG_INFO(RYU_USE_LOG_CATEGORY(Engine), "Shutting down Engine");

		m_app->Shutdown();
		Ryu::Graphics::Shutdown();

		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Shutdown Engine");
	}

	void Engine::SetCommandLine(std::wstring_view cmdLine)
	{
		m_cmdLine = Config::CommandLine(cmdLine);
	}

	f64 Engine::GetEngineUpTime()
	{
		return s_timer.GetTotalSeconds();
	}

	void Engine::Run()
	{
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Engine), "Running the Engine");

		if (m_app == nullptr)
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Engine application not initialized!");
			return;
		}

		if (!Init())
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Failed to initialize Engine! Exiting.");
			return;
		}

		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Starting Engine main loop");

		auto appWindow = m_app->GetWindow();
		while (m_app->IsRunning())
		{
			// Pump regardless of frame time
			appWindow->PumpMessages();

			s_timer.Tick([&]()
			{
				DoFrame(s_timer.GetElapsedSeconds());
			});
		}

		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Stopping Engine main loop");

		Shutdown();
	}

	void Engine::Quit() const noexcept
	{
		if (m_app)
		{
			LOG_INFO(RYU_USE_LOG_CATEGORY(Engine), "Rquesting application shutdown");
			m_app->StopRunning();
		}
	}

	void Engine::DoFrame(f64 dt) const
	{
		m_app->Tick(dt);
		Graphics::RenderSurface();
	}

	void Engine::OnAppResize(u32 width, u32 height) const noexcept
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine::OnAppResize -  {}x{}", width, height);
		RYU_TODO("Fire resize to the graphics engine")
	}
}
