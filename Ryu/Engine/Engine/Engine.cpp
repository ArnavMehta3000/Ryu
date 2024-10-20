#include "Engine.h"
#include "App/Application.h"
#include "App/Window.h"
#include "StepTimer/StepTimer.h"
#include "Graphics/Renderer.h"

namespace Ryu::Engine
{
	RYU_SINGLETON_IMPL(Engine);

	DX::StepTimer s_timer;

	Engine::Engine()
		: m_app(nullptr)
	{
	}

	Engine::~Engine() = default;

	void Engine::Init()
	{
		LOG_INFO(RYU_USE_LOG_CATEGORY(Engine), "Initializing Engine");

		// Initialize the application
		if (m_app->Init())
		{
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine application initialized successfully");
		}
		else
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Failed to initialize Engine application");
			return;
		}
		
		if (Ryu::Graphics::Init(Ryu::Graphics::API::DirectX12))
		{
			LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Graphics initialized successfully");
		}
		else
		{
			LOG_FATAL(RYU_USE_LOG_CATEGORY(Engine), "Failed to initialize Graphics");
			return;
		}

		m_renderSurface.Window = dynamic_cast<Ryu::App::Window*>(m_app->GetWindow().get());
		m_renderSurface.Surface = Graphics::CreateSurface(m_renderSurface.Window);

		ASSERT(m_renderSurface.Surface != nullptr, "Failed to create render surface");

		LOG_TRACE(RYU_USE_LOG_CATEGORY(Engine), "Engine initialized successfully");
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

	f64 Engine::GetEngineUpTime() const
	{
		return s_timer.GetTotalSeconds();
	}

	void Engine::Run()
	{
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(Engine), "Running the Engine");

		if (m_app == nullptr)
		{
			LOG_ERROR(RYU_USE_LOG_CATEGORY(Engine), "Engine application not initialized!");
			return;
		}

		Init();

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

	void Engine::DoFrame(MAYBE_UNUSED f64 dt)
	{
		m_app->OnTick(dt);
		Graphics::RenderSurface();
	}
}
