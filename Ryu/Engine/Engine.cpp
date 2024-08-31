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
		RYU_ENGINE_TRACE("Engine constructed");
	}

	Engine::~Engine()
	{
		m_application.reset();

		RYU_ENGINE_TRACE("Engine destructed");
	}

	bool Engine::PreInit()
	{
		RYU_ENGINE_DEBUG("Pre-initializing engine");

		// TODO: Load pre-init plugins

		RYU_ENGINE_TRACE("Finished pre-initializing engine");
		return true;
	}

	bool Engine::Init()
	{
		RYU_ENGINE_DEBUG("Initializing engine");

		if (!m_application->OnInit())
		{
			RYU_ENGINE_FATAL("Failed to initialize application!");
			return false;
		}

		RYU_ENGINE_TRACE("Finished initializing engine");
		return true;
	}

	bool Engine::PostInit()
	{
		RYU_ENGINE_DEBUG("Post-initializing engine");

		// TODO: Load post-init plugins

		RYU_ENGINE_TRACE("Finished post-initializing engine");
		return true;
	}

	void Engine::Run()
	{
		RYU_ENGINE_DEBUG("Running engine");

		PreInit();
		Init();
		PostInit();

		using ClockType = std::chrono::steady_clock;

		ClockType clock;
		std::chrono::time_point<ClockType> lastTime;
		auto timePoint1 = clock.now();
		auto timePoint2 = timePoint1;
		f32 deltaTime = 0.001f;

		while (m_application->m_window.IsOpen())
		{
			Tick(deltaTime);

			timePoint2 = clock.now();
			std::chrono::duration<f32> dt = timePoint2 - timePoint1;
			timePoint1 = timePoint2;
			deltaTime = dt.count();
		}

		RYU_ENGINE_DEBUG("Shutting down engine");
		m_application->OnShutdown();
	}

	void Engine::Tick(MAYBE_UNUSED f32 dt)
	{
		m_application->m_window.PumpMessages();

		m_application->OnUpdate(dt);
		m_application->OnRender();
	}
}
