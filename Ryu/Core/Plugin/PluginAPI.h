#pragma once
#include <Core/Includes.h>
#include <Core/Plugin/IPlugin.h>

#define PLUGIN_API RYU_API

RYU_API_FUNC(::Ryu::IPlugin*, CreatePlugin);
RYU_API_FUNC(void, DestroyPlugin, ::Ryu::IPlugin*);
RYU_API_FUNC(void, RegisterPlugin);

// Macro to register the plugin
#define RYU_REGISTER_PLUGIN(Plugin, ...)  \
Ryu::IPlugin* CreatePlugin()              \
{                                         \
	return new Plugin(__VA_ARGS__);       \
}                                         \
void DestroyPlugin(Ryu::IPlugin* plugin)  \
{                                         \
	if (plugin)                           \
	{                                     \
		delete (Plugin*)plugin;           \
		plugin = nullptr;                 \
	}                                     \
}                                         \
void RegisterPlugin()                     \
{                                         \
	Plugin::RegisterStaticPlugin();       \
}
