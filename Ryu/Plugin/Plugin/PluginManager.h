#pragma once
#include "Common/Common.h"
#include "Plugin/PluginInfo.h"
#include <expected>
#include <Windows.h>

namespace Ryu::Plugin
{
	class PluginManager
	{
		struct LoadedPlugin
		{
			HMODULE Handle;
			std::function<void()> ShutdownFunc;
			std::any FunctionTable;
		};

	public:
		~PluginManager();

		template <PluginInterface T>
		std::expected<typename T::FunctionTable, std::string> LoadPluginInterface(const std::string& pluginPath);

		template <PluginInterface T>
		std::optional<typename T::FunctionTable> GetPluginInterface();

		void UnLoadPluginInterface(const std::string& apiName);

	private:
		HMODULE LoadDLL(const std::string& pluginPath);
		void UnloadDLL(HMODULE handle);

		template <typename T>
		T GetDLLFunction(HMODULE handle, const std::string& functionName);

	private:
		std::unordered_map<std::string, LoadedPlugin> m_loadedPlugins;
	};


	template<typename T>
	inline T PluginManager::GetDLLFunction(HMODULE handle, const std::string& functionName)
	{
		if (!handle)
		{
			return nullptr;
		}
		return reinterpret_cast<T>(::GetProcAddress(handle, functionName.c_str()));
	}

	template<PluginInterface T>
	inline std::expected<typename T::FunctionTable, std::string> PluginManager::LoadPluginInterface(const std::string& pluginPath)
	{
		// Try to early return
		if (auto opt = GetPluginInterface<T>(); opt.has_value())
		{
			return opt.value();
		}

		// Load dll
		HMODULE handle = LoadDLL(pluginPath);
		if (!handle)
		{
			return std::unexpected("Failed to load plugin: " + pluginPath);
		}

		// Get creator func
		auto createPluginFunc = GetDLLFunction<PluginCreationFunction>(handle, "CreatePlugin");
		if (!createPluginFunc)
		{
			UnloadDLL(handle);
			return std::unexpected("Plugin missing 'CreatePlugin' function");
		}

		// Get destroyer func
		auto destroyPluginFunc = GetDLLFunction<PluginDestructionFunction>(handle, "DestroyPlugin");
		if (!destroyPluginFunc)
		{
			UnloadDLL(handle);
			return std::unexpected("Plugin missing 'DestroyPlugin' function");
		}
		
		// Create an instance of that plugin
		auto* pluginInstance = createPluginFunc();
		if (!pluginInstance) 
		{
			UnloadDLL(handle);
			return std::unexpected("Failed to create plugin instance");
		}

		// Get the function table from the plugin instance
		auto getFunctionTableFunc = GetDLLFunction<PluginFunctionTableGetterFunction<T>>(handle, "GetFunctionTable");
		if (!getFunctionTableFunc)
		{
			destroyPluginFunc(pluginInstance);
			UnloadDLL(handle);
			return std::unexpected("Plugin missing 'GetFunctionTable' function");
		}

		// Validate the function table
		typename T::FunctionTable functionTable;
		getFunctionTableFunc(pluginInstance, &functionTable);
		if (!T::Validate(functionTable))
		{
			destroyPluginFunc(pluginInstance);
			UnloadDLL(handle);
			return std::unexpected("Plugin function table validation failed");
		}

		// Store loaded plugin info
		LoadedPlugin lp;
		lp.Handle = handle;
		lp.FunctionTable = functionTable;
		lp.ShutdownFunc = [pluginInstance, destroyPluginFunc]()
		{ 
			destroyPluginFunc(pluginInstance);
		};
		

		m_loadedPlugins[T::GetInterfaceName()] = std::move(lp);

		return functionTable;
	}

	template<PluginInterface T>
	inline std::optional<typename T::FunctionTable> PluginManager::GetPluginInterface()
	{
		auto it = m_loadedPlugins.find(T::GetInterfaceName());
		if (it == m_loadedPlugins.end())
		{
			return std::nullopt;
		}

		try
		{
			return std::any_cast<typename T::FunctionTable>(it->second.FunctionTable);
		}
		catch (const std::bad_any_cast&)
		{
			return std::nullopt;
		}
	}

	using FunctionSignatures = std::unordered_map<std::string, std::any>;

	struct DLLTestFunctionTable
	{
		PluginFunctionR<bool, PluginPhase> MyInitialize;
		PluginFunction<PluginPhase> MyShutdown;
		PluginFunctionR<bool, bool, i32, f32, char32> TestFunc;
	};


	class DLLTestInterface : public IPluginInterface<DLLTestFunctionTable>
	{
	public:
		static std::string GetInterfaceName() { return "DLLTest"; }
		static std::unordered_map<std::string, std::any> GetFunctionSignatures()
		{
			return
			{
				{ "MyInitialize", std::any(PluginFunctionR<bool, PluginPhase>{}) },
				{ "MyShutdown", std::any(PluginFunction<PluginPhase>{})},
				{ "TestFunc", std::any(PluginFunctionR<bool, bool, i32, f32, char32>{}) },
			};
		}

		static bool Validate(const FunctionTable& ft)
		{
			return ft.MyInitialize && ft.MyShutdown && ft.TestFunc;
		}
	};

	static_assert(PluginInterface<DLLTestInterface>);
}


#define RYU_PLUGIN(PluginName, InterfaceName)                                                                 \
static_assert(PluginImplementation<PluginName, InterfaceName>);                                               \
extern "C"                                                                                                    \
{                                                                                                             \
	__declspec(dllexport) PluginName* CreatePlugin()                                                          \
	{                                                                                                         \
		return new PluginName();                                                                              \
	}                                                                                                         \
                                                                                                              \
	__declspec(dllexport) void DestroyPlugin(void* plugin)                                                    \
	{                                                                                                         \
		delete (PluginName*)plugin;                                                                           \
		plugin = nullptr;                                                                                     \
	}                                                                                                         \
	__declspec(dllexport) bool GetFunctionTable(void* plugin, PluginName::Interface::FunctionTable* outTable) \
	{                                                                                                         \
		if (!plugin || !outTable)                                                                             \
		{                                                                                                     \
			return false;                                                                                     \
		}                                                                                                     \
                                                                                                              \
		if (auto p = static_cast<PluginName*>(plugin))                                                        \
		{                                                                                                     \
			*outTable = PluginName::GetFunctionTable(*p);                                                     \
			return true;                                                                                      \
		}                                                                                                     \
		return false;                                                                                         \
	}                                                                                                         \
}
