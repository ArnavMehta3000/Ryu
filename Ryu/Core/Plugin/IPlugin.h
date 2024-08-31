#pragma once
#include <Core/StandardTypes.h>
#include <Windows.h>
#include <string>

namespace Ryu
{
	enum class PluginLoadOrder
	{
		PreInit,
		PostInit,
		Default = PostInit,
	};

	enum class PluginTickOrder
	{
		PreUpdate,
		PostUpdate,
		Default = PostUpdate
	};

	enum class PluginRenderOrder
	{
		PreRender,
		PostRender,
		Default = PostRender
	};

	struct RYU_API PluginAPI
	{
		HWND Window{ nullptr };
	};

	class RYU_API IPlugin
	{
	public:
		IPlugin(PluginLoadOrder loadOrder = PluginLoadOrder::Default) 
			: m_loadOrder(loadOrder) {}

		virtual ~IPlugin() = default;
		virtual bool Initialize(const PluginAPI& api) = 0;
		virtual void Shutdown() = 0;
		inline PluginLoadOrder GetLoadOrder() const { return m_loadOrder; }

	private:
		PluginLoadOrder m_loadOrder;
	};

	class ITickablePlugin : public IPlugin
	{
	public:
		ITickablePlugin(
			PluginTickOrder tickOrder = PluginTickOrder::Default, 
			PluginLoadOrder loadOrder = PluginLoadOrder::Default) : IPlugin(loadOrder)
			, m_tickOrder(tickOrder) {}

		virtual void OnUpdate(const f32 dt) = 0;

	private:
		PluginTickOrder m_tickOrder;
	};

	class IRenderablePlugin : public IPlugin
	{
	public:
		IRenderablePlugin(
			PluginRenderOrder renderOrder = PluginRenderOrder::Default, 
			PluginLoadOrder loadOrder = PluginLoadOrder::Default) : IPlugin(loadOrder)
			, m_renderOrder(renderOrder) {}

		virtual void OnRender(const f32 dt) = 0;

	private:
		PluginRenderOrder m_renderOrder;
	};

	class ITickableRenderablePlugin : public IPlugin
	{
	public:
		ITickableRenderablePlugin(
			PluginTickOrder tickOrder = PluginTickOrder::Default, 
			PluginRenderOrder renderOrder = PluginRenderOrder::Default, 
			PluginLoadOrder loadOrder = PluginLoadOrder::Default) : IPlugin(loadOrder)
			, m_tickOrder(tickOrder)
			, m_renderOrder(renderOrder) {}

		virtual void OnUpdate(const f32 dt) = 0;
		virtual void OnRender(const f32 dt) = 0;

	private:
		PluginTickOrder m_tickOrder;
		PluginRenderOrder m_renderOrder;
	};
};