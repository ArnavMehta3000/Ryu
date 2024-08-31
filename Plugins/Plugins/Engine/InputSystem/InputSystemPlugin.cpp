#pragma once
#include <InputSystem/InputSystem.h>
#include <InputSystem/Log.h>

Ryu::IPlugin* CreatePlugin()
{
	RYU_PLUGIN_INFO("Created InputSystem plugin");
	return new Ryu::InputSystem();
}

void DestroyPlugin(Ryu::IPlugin* plugin)
{
	if (plugin)
	{
		RYU_PLUGIN_INFO("Destroyed InputSystem plugin");
		delete plugin;
		plugin = nullptr;
	}
}