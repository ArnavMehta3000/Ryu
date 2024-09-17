#include "Test.h"
#include <PluginData.h>
#include <Plugins/Engine/RyuTest/Internal/Log.h>

RYU_REGISTER_PLUGIN(RyuTest)

RyuTest::RyuTest() : Ryu::PluginBase<RyuTest>()
{
	RYU_PLUGIN_TRACE("Constructed RyuInput plugin");

		if (!GetInstance())
		{
			SetPluginInstance(this);
		}
}

RyuTest::~RyuTest()
{
}

bool RyuTest::Initialize(const Ryu::PluginAPI &api)
{
	return true;
}

void RyuTest::Shutdown() {}
