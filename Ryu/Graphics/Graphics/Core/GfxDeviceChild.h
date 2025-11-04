#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class Device;

	class DeviceChild
	{
		friend class Device;
	public:
		explicit DeviceChild(Device* device);  // Defined in Device.cpp
		virtual ~DeviceChild();  // Defined in Device.cpp

		virtual void ReleaseObject() { /* Can be overridden if needed */ }
		[[nodiscard]] inline Device* GetDevice() const noexcept { return m_device; }

	private:
		inline void DisconnectFromDevice() { m_isRegistered = false; }

	private:
		Device* m_device = nullptr;
		bool m_isRegistered : 1 = false;
	};
}
