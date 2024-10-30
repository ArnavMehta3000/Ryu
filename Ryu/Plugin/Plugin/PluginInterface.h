#pragma once
#include "Plugin/API.h"
#include "Common/Common.h"
#include <string_view>

namespace Ryu::Plugin
{
	class RYU_PLUGIN_API PluginInterface
	{
	public:
		PluginInterface();
		PluginInterface(const PluginInterface&) = delete;
		PluginInterface(PluginInterface&&) = delete;
		virtual ~PluginInterface() = default;

		bool Activate();
		void Deactivate();

		bool IsActive() const noexcept;

		virtual std::string_view GetDescription() const noexcept = 0;
		virtual std::string_view GetName() const noexcept = 0;
		virtual u32 GetVersion() const noexcept = 0;

		PluginInterface& operator=(const PluginInterface&) = delete;
		PluginInterface& operator=(PluginInterface&&) = delete;

	private:
		virtual bool OnActivate() = 0;
		virtual void OnDeactivate() = 0;

	private:
		bool m_isActive;
	};


	template<typename T>
	struct StaticPluginProvider;
}

