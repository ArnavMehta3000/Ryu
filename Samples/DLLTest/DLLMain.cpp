#include "Plugin/PluginManager.h"
#include <print>

using namespace Ryu::Plugin;

class DLLTestPlugin
{
public:
	using Interface = DLLTestInterface;

	DLLTestPlugin() { x = 10; }
	bool OnInitialize(PluginPhase) { std::print("PLUGIN TEST!");  return true; }
	void OnShutdown(PluginPhase) {}

	bool TestFunc(bool, i32, f32, char32) { return true; }

	static PluginInfo GetInfo() { return PluginInfo{ .Version = "1.0 "}; }
	static Interface::FunctionTable GetFunctionTable(DLLTestPlugin& self)
	{
		return Interface::FunctionTable
		{
			.MyInitialize = [&self](PluginPhase phase) { return self.OnInitialize(phase); },
			.MyShutdown = [&self](PluginPhase phase) { self.OnShutdown(phase); },
			.TestFunc = [&self](bool b, i32 i, f32 f, char32 c) { return self.TestFunc(b, i, f, c); },
		};
	}

private:
	int x = 5;
};


#define PLUGIN(PluginName, InterfaceName)                                         \
	static_assert(PluginImplementation<PluginName, InterfaceName>);               \
	extern "C"                                                                    \
		{                                                                         \
		__declspec(dllexport)                                                     \
		PluginName* CreatePlugin()                                                \
		{                                                                         \
			return new PluginName();                                              \
		}                                                                         \
                                                                                  \
		__declspec(dllexport)                                                     \
		void DestroyPlugin(void* plugin)                                          \
		{                                                                         \
			delete (PluginName*)plugin;                                           \
			plugin = nullptr;                                                     \
		}                                                                         \
		__declspec(dllexport)                                                     \
		PluginName::Interface::FunctionTable GetFunctionTable(void* plugin) \
		{                                                                         \
			return PluginName::Interface::GetFunctionTable();          \
		}                                                                         \
	}




//PLUGIN(DLLTestPlugin, DLLTestInterface)


static_assert(PluginImplementation<DLLTestPlugin, DLLTestInterface>);
extern "C"
{
	__declspec(dllexport)
		DLLTestPlugin* CreatePlugin()
	{
		return new DLLTestPlugin();
	}

	__declspec(dllexport)
		void DestroyPlugin(void* plugin)
	{
		delete (DLLTestPlugin*)plugin;
		plugin = nullptr;
	}
	__declspec(dllexport)
		bool GetFunctionTable(void* plugin, DLLTestPlugin::Interface::FunctionTable* outTable)
	{
		if (!plugin || !outTable)
			return false;

		if (auto p = static_cast<DLLTestPlugin*>(plugin))
		{
			*outTable = DLLTestPlugin::GetFunctionTable(*p);
			return true;
		}
		return false;
	}
}
