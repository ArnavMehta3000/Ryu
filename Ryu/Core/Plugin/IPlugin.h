#pragma once
#include <Core/Application/IApplication.h>
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
		None,
		PreUpdate,
		PostUpdate,
		Default = PostUpdate
	};

	enum class PluginRenderOrder
	{
		None,
		PreRender,
		PostRender,
		Default = PostRender
	};

	struct RYU_API PluginAPI
	{
		IApplication* App{ nullptr };
	};

	class RYU_API IPlugin
	{
	public:
		IPlugin() = default;
		virtual ~IPlugin() = default;
		virtual bool Initialize(const PluginAPI& api) = 0;
		virtual void OnUpdate(MAYBE_UNUSED const f32 dt) {};
		virtual void OnRender(MAYBE_UNUSED const f32 dt) {};
		virtual void Shutdown() = 0;
	};
};