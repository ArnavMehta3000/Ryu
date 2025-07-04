#pragma once
#include "Common/Concepts.h"
#include <any>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

namespace Ryu::Plugin
{
	struct PluginInfo
	{
		std::string Name;
		std::string Version;
		std::string Description;
		std::vector<std::string> Dependencies;
	};

	enum class PluginPhase
	{
		OnLoad,
		OnPreInit,
		OnInitialize,
		OnShutdown,
		OnPostShutdown,
		OnUnload
	};

	template <typename... Args>
	using PluginFunction = std::function<void(Args...)>;

	template <typename R, typename... Args>
	using PluginFunctionR = std::function<R(Args...)>;

	template <typename T>
	concept PluginInterface = requires(typename T::FunctionTable ft)
	{
		typename T::FunctionTable;
		{ T::GetInterfaceName()      } -> std::convertible_to<std::string>;
		{ T::GetFunctionSignatures() } -> std::convertible_to<std::unordered_map<std::string, std::any>>;
		{ T::Validate(ft)            } -> std::same_as<bool>;
	};

	template <typename PluginType, typename InterfaceType>
	concept PluginImplementation = requires(PluginType plugin, PluginPhase phase)
	{
		{ PluginType::GetInfo()                 } -> std::same_as<PluginInfo>;
	    { PluginType::GetFunctionTable(plugin)  } -> std::convertible_to<typename InterfaceType::FunctionTable>;
	    { plugin.OnInitialize(phase)            } -> std::same_as<bool>;
	    { plugin.OnShutdown(phase)              } -> std::same_as<void>;
	};

	using PluginCreationFunction = std::add_pointer_t<void*()>;
	using PluginDestructionFunction = std::add_pointer_t<void(void*)>;

	template <typename T>
	using PluginFunctionTableGetterFunction = std::add_pointer_t<bool(void*, typename T::FunctionTable*)>;


	// Base for declaring template implementations
	template <typename InterfaceType>
	struct IPlugin
	{
		using Interface = InterfaceType;
	};

	// Base class for declaring plugin interfaces
	template <typename FuncTable>
	struct IPluginInterface
	{
		using FunctionTable = FuncTable;
	};
}
