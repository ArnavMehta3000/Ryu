#pragma once
#include "Common/Common.h"
#include "Config/CommandLine.h"
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

		Engine& SetApp(std::shared_ptr<App::Application> app) { m_app = std::move(app); return Engine::Get(); }
		Engine& SetCommandLine(LPWSTR cmdLine);
		
		inline std::shared_ptr<App::Application> GetApp() const { return m_app; }
		inline const Config::CommandLine& GetCommdandLine() const { return m_cmdLine; }

		f64 GetEngineUpTime() const;

		void Run();

	private:
		Engine();
		void Init();
		void Shutdown();
		void DoFrame(MAYBE_UNUSED f64 dt);

	private:
		std::shared_ptr<App::Application> m_app;
		Config::CommandLine m_cmdLine;
	};
}
