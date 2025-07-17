#pragma once
#include "Plugin/PluginManager.h"
#include "Logger/Logger.h"
#include <functional>

namespace Ryu::App { class App; }
namespace Ryu::Gfx { class Renderer; }

namespace Ryu::Engine
{
	class Engine;

	struct EngineContext
	{
		std::function<Logging::Logger*()> GetLogger;
		std::function<App::App*()> GetApp;
		std::function<Gfx::Renderer*()> GetRenderer;
	};

	struct EnginePluginFunctions
	{
		Plugin::PluginFunctionR<bool, Plugin::PluginPhase, const void*> Initialize;
		Plugin::PluginFunction<Plugin::PluginPhase> Shutdown;
		Plugin::PluginFunctionR<std::string> GetName;
	};

	class EnginePluginInterface : public Plugin::IPluginInterface<EnginePluginFunctions>
	{
	public:
		static std::string GetInterfaceName() { return "RyuEnginePlugin"; }
		static Plugin::FunctionSignatures GetFunctionSignatures();
		static bool Validate(const FunctionTable& ft);
	};
	static_assert(Plugin::PluginInterface<EnginePluginInterface>);



	// Base class for all engine plugins
	class EnginePluginBase : public Plugin::IPlugin<EnginePluginInterface>
	{
	public:
		virtual ~EnginePluginBase() = default;
		virtual bool OnInitialize(Plugin::PluginPhase, const void* ctx = nullptr);
		virtual void OnShutdown(Plugin::PluginPhase) {}
		virtual std::string GetPluginName() const = 0;

		static Interface::FunctionTable GetFunctionTable(EnginePluginBase& self);

		void LogMessage(const Logging::LogCategory& category, Logging::LogLevel level, std::string message, std::stacktrace st = std::stacktrace::current());
		App::App * GetApplication();
		Gfx::Renderer* GetRenderer();

	private:
		const EngineContext* m_engineCtx = nullptr;
	};
}
