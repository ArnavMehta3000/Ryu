#pragma once
#include <InputSystem/InputSystem.h>
#include <InputSystem/Log.h>

Ryu::IPlugin* CreatePlugin()
{
	return new Ryu::InputSystem();
}

void DestroyPlugin(Ryu::IPlugin* plugin)
{
	if (plugin)
	{
		delete plugin;
		plugin = nullptr;
	}
}