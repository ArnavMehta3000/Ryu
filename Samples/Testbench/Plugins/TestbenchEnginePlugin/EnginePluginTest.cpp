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
	bool OnInitialize(PluginPhase phase, const EngineContext* ctx) override
	{
		if (EnginePluginBase::OnInitialize(phase, ctx))
		{
			this->LogMessage(LogEnginePluginTest, LogLevel::Warn, "TestbenchEnginePlugin initialized.");
			return true;
		}

		return false;
	}

	virtual void OnShutdown(PluginPhase) {}

	std::string GetPluginName() const { return "TestbenchEnginePlugin"; }

	static PluginInfo GetInfo() { return PluginInfo{ .Version = "1.0 " }; }
};


RYU_PLUGIN(TestbenchEnginePlugin, EnginePluginInterface)
