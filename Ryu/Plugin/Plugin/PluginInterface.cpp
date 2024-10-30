#include "PluginInterface.h"

namespace Ryu::Plugin
{
	PluginInterface::PluginInterface()
		: m_isActive(false)
	{}
	
	bool PluginInterface::Activate()
	{
		if (m_isActive)
		{
			return true;
		}
		
		m_isActive = OnActivate();
		return m_isActive;
	}
	
	void PluginInterface::Deactivate()
	{
		if (!m_isActive)
		{
			return;
		}

		OnDeactivate();
		m_isActive = false;
	}
	
	bool PluginInterface::IsActive() const noexcept
	{
		return m_isActive;
	}
}
