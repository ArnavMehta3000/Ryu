#pragma once
#include <string>

namespace Ryu
{
	enum class PluginLoadOrder
	{
		PreInit,
		PostInit,
		Default = PostInit,
	};

	class IPlugin
	{
	public:
		IPlugin(PluginLoadOrder loadOrder = PluginLoadOrder::Default) 
			: m_loadOrder(loadOrder) {}
		virtual ~IPlugin() = default;

		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual std::string GetName() const = 0;

		inline PluginLoadOrder GetLoadOrder() const { return m_loadOrder; }

	private:
		PluginLoadOrder m_loadOrder;
	};

};