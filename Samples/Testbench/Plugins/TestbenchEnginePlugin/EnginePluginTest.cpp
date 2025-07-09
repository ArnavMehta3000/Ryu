#include "Engine/Plugin/EnginePlugin.h"
#include "Logger/Logger.h"
#include "Engine/Engine.h"

using namespace Ryu::Engine;
using namespace Ryu::Logging;
using namespace Ryu::Plugin;

class TestbenchEnginePlugin : public EnginePluginBase
{
	RYU_LOG_DECLARE_CATEGORY(EnginePluginTest);

public:
	bool OnInitialize(PluginPhase phase, const void* ctx) override
	{
		if (EnginePluginBase::OnInitialize(phase, ctx))
		{
			auto log = [this](std::string_view msg) { LogMessage(LogEnginePluginTest, LogLevel::Debug, msg.data()); };

			if (phase == PluginPhase::OnLoad)
			{
				log("TestbenchEnginePlugin OnLoad");
			}
			else if (phase == PluginPhase::OnInitialize)
			{
				log("TestbenchEnginePlugin OnInitialize");
			}
			
			return true;
		}

		return false;
	}

	virtual void OnShutdown(PluginPhase phase) 
	{
		auto log = [this](std::string_view msg) { LogMessage(LogEnginePluginTest, LogLevel::Debug, msg.data()); };

		if (phase == PluginPhase::OnShutdown)
		{
			log("TestbenchEnginePlugin OnShutdown");
		}
		else if (phase == PluginPhase::OnUnload)
		{
			log("TestbenchEnginePlugin OnUnload");
		}

		EnginePluginBase::OnShutdown(phase);
	}

	std::string GetPluginName() const { return "TestbenchEnginePlugin"; }

	static PluginInfo GetInfo() { return PluginInfo{ .Version = "1.0 " }; }
};


RYU_PLUGIN(TestbenchEnginePlugin, EnginePluginInterface)
