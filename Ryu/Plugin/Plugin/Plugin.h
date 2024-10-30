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
		RYU_DISABLE_COPY(Plugin);

	public:
		Plugin(DLL dll, std::unique_ptr<T> pluginInterface)
			: m_interface(std::move(pluginInterface))
			, m_dll(std::move(dll))
		{}

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
			return Plugin<U>(std::move(m_dll), std::unique_ptr<U>(std::static_pointer_cast<U>(m_interface.release())));
		}

		bool Activate()
		{
			return m_interface ? m_interface->Activate() : false;
		}
		
		void Deactivate()
		{
			if (m_interface)
			{
				m_interface->Deactivate();
			}
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

	private:
		std::unique_ptr<T> m_interface;
		DLL m_dll;
	};

	using GenericPlugin = Plugin<PluginInterface>;
}
