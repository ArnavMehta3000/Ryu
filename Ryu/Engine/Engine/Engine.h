#pragma once
#include "Config/CommandLine.h"
#include "Logger/Logger.h"
#include "Graphics/ISurface.h"
#include "Event/Event.h"
#include <memory>

namespace Ryu::App { class Application; }

namespace Ryu::Engine
{
	class Engine
	{
		RYU_DECLARE_SINGLETON(Engine, true);
		RYU_LOG_CATEGORY(Engine);

	public:
		Engine();
		~Engine();

		void SetApp(std::shared_ptr<App::Application> app) { m_app = std::move(app); }
		void SetCommandLine(std::wstring_view cmdLine);
		
		inline std::shared_ptr<App::Application> GetApp() const { return m_app; }
		inline const Config::CommandLine& GetCommdandLine() const { return m_cmdLine; }

		static f64 GetEngineUpTime();

		void Run();
		void Quit() const noexcept;

	private:
		bool Init();
		void Shutdown();
		void DoFrame(f64 dt) const;

	private:
		std::shared_ptr<App::Application> m_app;
		Config::CommandLine               m_cmdLine;
		Graphics::GFXSurface              m_renderSurface;
	};
}
