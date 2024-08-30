#include "Engine.h"
#include <Engine/Log.h>
#include <chrono>


namespace Ryu
{
	RYU_API Engine& GetEngineInstance()
	{
		return Engine::Get();
	}

	Engine::Engine()
		: m_application(nullptr)
	{

	}

	Engine::~Engine()
	{
		m_application.reset();
	}

	void Engine::Run()
	{
		if (!m_application->OnInit())
		{
			RYU_ENGINE_FATAL("Failed to initialize application!");
			return;
		}

		RYU_ENGINE_DEBUG("Running application");

		using ClockType = std::chrono::steady_clock;

		ClockType clock;
		std::chrono::time_point<ClockType> lastTime;
		auto timePoint1 = clock.now();
		auto timePoint2 = timePoint1;
		f32 deltaTime = 0.001f;

		while (m_application->m_window.IsOpen())
		{
			m_application->m_window.PumpMessages();

			m_application->OnUpdate(deltaTime);
			m_application->OnRender();

			timePoint2 = clock.now();
			std::chrono::duration<f32> dt = timePoint2 - timePoint1;
			timePoint1 = timePoint2;
			deltaTime = dt.count();
		}

		RYU_ENGINE_DEBUG("Shutting down application");
		m_application->OnShutdown();
	}
}
