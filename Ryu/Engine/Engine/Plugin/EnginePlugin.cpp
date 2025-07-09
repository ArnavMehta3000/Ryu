#include "Engine/Plugin/EnginePlugin.h"
#include "Logger/Assert.h"
#include <print>

namespace Ryu::Engine
{
	using namespace Plugin;

	FunctionSignatures EnginePluginInterface::GetFunctionSignatures()
	{
		return
		{
			{ "Initialize", std::any(PluginFunctionR<bool, PluginPhase, const EngineContext*>{}) },
			{ "Shutdown", std::any(PluginFunction<PluginPhase>{}) },
			{ "GetName", std::any(PluginFunctionR<std::string>{}) }
		};
	}

	bool EnginePluginInterface::Validate(const FunctionTable& ft)
	{
		return ft.Initialize
			&& ft.Shutdown
			//&& ft.Update
			//&& ft.Render 
			&& ft.GetName;
	}

	bool EnginePluginBase::OnInitialize(Plugin::PluginPhase, const void* ctx)
	{
		// Only set the engine context once 
		if (!m_engineCtx && ctx)
		{
			m_engineCtx = static_cast<const EngineContext*>(ctx);
		}

		return true;
	}

	void EnginePluginBase::LogMessage(const Logging::LogCategory& category, Logging::LogLevel level, std::string message, std::stacktrace st)
	{
		RYU_ASSERT(m_engineCtx, "Engine context for plugin is nullptr! Did you forget to call EnginePluginBase::OnInitialize?");

		if (auto logger = m_engineCtx->GetLogger())
		{
			logger->Log(category, level, { std::move(message), std::move(st) });
		}
		else
		{
			std::print("!!>>> NO LOGGER FOUND IN ENGINE CONTEXT FOR PLUGIN - {}! <<<!!", GetPluginName());
		}
	}

	EnginePluginBase::Interface::FunctionTable EnginePluginBase::GetFunctionTable(EnginePluginBase& self)
	{
		using namespace Plugin;
		return Interface::FunctionTable
		{
			.Initialize = [&self](PluginPhase phase, const void* ctx) { return self.OnInitialize(phase, ctx); },
			.Shutdown   = [&self](PluginPhase phase)                  { self.OnShutdown(phase);               },
			.GetName    = [&self]()                                   { return self.GetPluginName();          }
		};
	}

	App::App* EnginePluginBase::GetApplication() 
	{
		return m_engineCtx ? m_engineCtx->GetApp() : nullptr;
	}
	
	Gfx::Renderer* EnginePluginBase::GetRenderer() 
	{
		return m_engineCtx ? m_engineCtx->GetRenderer() : nullptr;
	}
}
