#include "Engine.h"
#include "Engine/Runtime/Runtime.h"
#include "App/Window.h"
#include "GraphicsRHI/Config.h"
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
		: m_runtime(nullptr)
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
		if (InitRuntime())
		{
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine application initialized successfully");
		}
		else
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Failed to initialize Engine application");
			return false;
		}
		
		Graphics::API api = Graphics::GraphicsConfig::Get().GraphicsAPI;
		m_renderer = std::make_unique<Graphics::Renderer>();

		if (VoidResult result = Graphics::InitGraphics(m_renderer.get(), api, m_runtime->GetWindow()->GetHWND()); !result)
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Failed to initialize Graphics. Error: {}", result.error());
		}
		else
		{
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Graphics ({}) initialized successfully", EnumToString(api));
		}

		//m_renderSurface.Window = m_runtime->GetWindow().get();
		//m_renderSurface.Surface = Graphics::CreateSurface(m_renderSurface.Window);

		//DEBUG_ASSERT(m_renderSurface.Surface != nullptr, "Failed to create render surface");


		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine initialized successfully");

		return true;
	}

	bool Engine::InitRuntime()
	{
		// Bind the events before initializing the application
		std::ignore = m_runtime->OnWindowResize.Add([this](const App::Events::OnWindowResize& event)
		{
			OnAppResize(event.Width, event.Height);
		});

		// Init the application
		return m_runtime->Init();
	}

	void Engine::Shutdown()
	{
		LOG_INFO(RYU_USE_LOG_CATEGORY(Engine), "Shutting down Engine");

		m_runtime->Shutdown();
		Graphics::ShutdownGraphics(m_renderer.get());

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

		if (m_runtime == nullptr)
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

		// Show the window
		auto appWindow = m_runtime->GetWindow();

		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Showing window");
		appWindow->ShowWindow();

		while (m_runtime->IsRunning())
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
		if (m_runtime)
		{
			LOG_INFO(RYU_USE_LOG_CATEGORY(Engine), "Rquesting application shutdown");
			m_runtime->StopRunning();
		}
	}

	void Engine::DoFrame(f64 dt)
	{
		m_runtime->Tick(dt);
		m_renderer->BeginFrame();
		m_renderer->EndFrame();
	}

	void Engine::OnAppResize(u32 width, u32 height) const noexcept
	{
		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine::OnAppResize -  {}x{}", width, height);

		//if (Graphics::IsInitialized())
		//{
		//	Graphics::ResizeSurface(width, height);
		//}
	}
	
	std::string_view GetEngineDirectory()
	{
		return RYU_ENGINE_DIR;
	}
}
