#include "Engine.h"
#include "App/Application.h"
#include "App/Window.h"
#include "StepTimer/StepTimer.h"
#include "Graphics/Renderer.h"

namespace Ryu::Engine
{
	DX::StepTimer s_timer;

	Engine& Engine::Get()
	{
		static Engine engine;
		return engine;
	}

	Engine::Engine()
		: m_app(nullptr)
	{
	}

	void Engine::Init()
	{
		Ryu::Graphics::Init(Ryu::Graphics::API::DirectX12);
		m_app->Init();
	}

	void Engine::Shutdown()
	{
		m_app->Shutdown();
		Ryu::Graphics::Shutdown();
	}

	Engine& Engine::SetCommandLine(std::wstring_view cmdLine)
	{
		m_cmdLine = Config::CommandLine(cmdLine);
		return *this;
	}

	f64 Engine::GetEngineUpTime() const
	{
		return s_timer.GetTotalSeconds();
	}

	void Engine::Run()
	{
		if (m_app == nullptr)
		{
			return;
		}

		Init();

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

		Shutdown();
	}

	void Engine::DoFrame(MAYBE_UNUSED f64 dt)
	{
		m_app->OnTick(dt);
		Graphics::Render();
	}
}
