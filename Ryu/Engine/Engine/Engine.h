#pragma once
#include "Common/Common.h"
#include "Utils/Singleton.h"
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
		RYU_DECLARE_SINGLETON(Engine);
	public:
		Engine();
		~Engine();

		void SetApp(std::shared_ptr<App::Application> app) { m_app = std::move(app); }
		void SetCommandLine(std::wstring_view cmdLine);
		
		inline std::shared_ptr<App::Application> GetApp() const { return m_app; }
		inline const Config::CommandLine& GetCommdandLine() const { return m_cmdLine; }

		f64 GetEngineUpTime() const;

		void Run();

	private:
		void Init();
		void Shutdown();
		void DoFrame(MAYBE_UNUSED f64 dt);

	private:
		std::shared_ptr<App::Application> m_app;
		Config::CommandLine m_cmdLine;
	};
}
