#pragma once
#include "Plugin/PluginInterface.h"
#include "Plugin/DLL.h"
#include <memory>

namespace Ryu::Plugin
{
	template<typename T>
	class Plugin
	{
		static_assert(IsDerived<PluginInterface, T> && "Plugin must derive from Ryu::Engine::PluginInterface");

	public:
		Plugin(const Plugin&) = delete;
		Plugin(Plugin&&) noexcept = default;

		Plugin(DLL dll, T* pluginInterface)
			: m_dll(std::move(dll))
		{
			m_interface = std::unique_ptr<T>(pluginInterface);
		}

		~Plugin()
		{
			if (m_interface)
			{
				Deactivate();
				m_interface.reset();
			}
			m_dll.Unload();
		}

		template <typename U>
		Plugin<U> As()&&
		{
			//Plugin<U>(std::move(m_lib), std::unique_ptr<U>(SafeCast<U*>(m_interface.release())));
			return Plugin<U>(std::move(m_dll), std::unique_ptr<U>(static_cast<U*>(m_interface.release())));
		}

		bool Activate()
		{
			return m_interface ? static_cast<PluginInterface*>(m_interface.get())->Activate() : false;
		}
		
		void Deactivate()
		{
			m_interface->Deactivate();
		}

		inline const DLL& GetDLL() const noexcept { return m_dll; }
		inline T& GetInterface() noexcept { return *m_interface; }
		inline const T& GetInterface() const noexcept { return *m_interface; }

		T* operator->() noexcept { return m_interface.get(); }
		const T* operator->() const noexcept { return m_interface.get(); }

		operator Plugin<PluginInterface>()&&
		{
			return std::move(*this).As<PluginInterface>();
		}

		Plugin& operator=(const Plugin&) = delete;
		Plugin& operator=(Plugin&&) noexcept = default;

	private:
		std::unique_ptr<T> m_interface;
		DLL m_dll;
	};

	using GenericPlugin = Plugin<PluginInterface>;
}
