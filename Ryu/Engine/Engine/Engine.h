#pragma once
#include "Common/Common.h"
#include <memory>

namespace Ryu::App
{
	class Application;
}

namespace Ryu::Engine
{
	class Engine
	{
	public:
		static Engine& Get();
		~Engine() = default;

		inline std::shared_ptr<App::Application> GetApp() const { return m_app; }
		inline Engine& SetApp(std::shared_ptr<App::Application> app) { m_app = std::move(app); return Engine::Get(); }
		f64 GetEngineUpTime() const;

		void Run();

	private:
		Engine();
		void Init();
		void Shutdown();
		void DoFrame(MAYBE_UNUSED f64 dt);

	private:
		std::shared_ptr<App::Application> m_app;
	};
}
