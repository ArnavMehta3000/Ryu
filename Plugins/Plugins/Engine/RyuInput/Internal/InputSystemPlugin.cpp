#pragma once
#include <Plugins/Engine/RyuInput/InputSystem.h>

Ryu::IPlugin* CreatePlugin()
{
	return new Ryu::Input::InputSystem();
}

void DestroyPlugin(Ryu::IPlugin* plugin)
{
	if (plugin)
	{
		delete plugin;
		plugin = nullptr;
	}
}