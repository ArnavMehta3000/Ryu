#pragma once
#include "Plugin/PluginManager.h"
#include "Logger/Logger.h"
#include <functional>

namespace Ryu::Engine
{
	class Engine;

	struct EngineContext
	{
		std::function<Logging::Logger*()> GetLogger;
	};

	struct EnginePluginFunctions
	{
		Plugin::PluginFunctionR<bool, Plugin::PluginPhase, const EngineContext*> Initialize;
		Plugin::PluginFunction<Plugin::PluginPhase> Shutdown;
		//Pluign::PluginFunction<float> Update;
		//Pluign::PluginFunction<> Render;
		Plugin::PluginFunctionR<std::string> GetName;
	};

	class EnginePluginInterface : public Plugin::IPluginInterface<EnginePluginFunctions>
	{
	public:
		static std::string GetInterfaceName() { return "RyuEnginePlugin"; }

		static std::unordered_map<std::string, std::any> GetFunctionSignatures()
		{
			using namespace Plugin;

			return 
			{
				{ "Initialize", std::any(PluginFunctionR<bool, PluginPhase,const EngineContext*>{}) },
				{ "Shutdown", std::any(PluginFunction<PluginPhase>{}) },
				//{ "Update", std::any(PluginFunction<float>{}) },
				//{ "Render", std::any(PluginFunction<>{}) },
				{ "GetName", std::any(PluginFunctionR<std::string>{}) }
			};
		}

		static bool Validate(const FunctionTable& ft)
		{
			return ft.Initialize 
				&& ft.Shutdown 
				//&& ft.Update
				//&& ft.Render 
				&& ft.GetName;
		}
	};
	static_assert(Plugin::PluginInterface<EnginePluginInterface>);


	// Base class for all engine plugins
	class EnginePluginBase : public Plugin::IPlugin<EnginePluginInterface>
	{
	public:
		virtual ~EnginePluginBase() = default;
		virtual bool OnInitialize(Plugin::PluginPhase, const EngineContext* ctx) { m_engineCtx = ctx; return true; }
		virtual void OnShutdown(Plugin::PluginPhase) {}
		//virtual void OnUpdate(float deltaTime) {}
		//virtual void OnRender() {}
		virtual std::string GetPluginName() const = 0;

		void LogMessage(const Logging::LogCategory& category, Logging::LogLevel level, std::string message, std::stacktrace st = std::stacktrace::current())
		{
			if (m_engineCtx && m_engineCtx->GetLogger)
			{
				if (auto logger = m_engineCtx->GetLogger())
				{
					logger->Log(category, level, { std::move(message), std::move(st) });
				}
			}
		}

		static inline Interface::FunctionTable GetFunctionTable(EnginePluginBase& self)
		{
			using namespace Plugin;
			return Interface::FunctionTable
			{
				.Initialize = [&self](PluginPhase phase, const EngineContext* ctx) { return self.OnInitialize(phase, ctx); },
				.Shutdown   = [&self](PluginPhase phase)                           { self.OnShutdown(phase);               },
				//.Update   = [&self](float deltaTime)                             { self.OnUpdate(deltaTime);             },
				//.Render   = [&self]()                                            { self.OnRender();                      },
				.GetName    = [&self]()                                            { return self.GetPluginName();          }
			};
		}

	private:
		const EngineContext* m_engineCtx = nullptr;
	};
}
