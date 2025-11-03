#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GfxDevice;

	class GfxDeviceChild
	{
		friend class GfxDevice;
	public:
		explicit GfxDeviceChild(GfxDevice* device);  // Defined in GfxDevice.cpp
		virtual ~GfxDeviceChild();  // Defined in GfxDevice.cpp

		virtual void ReleaseObject() { /* Can be overridden if needed */ }
		[[nodiscard]] inline GfxDevice* GetDevice() const noexcept { return m_device; }

	private:
		inline void DisconnectFromDevice() { m_isRegistered = false; }

	private:
		GfxDevice* m_device = nullptr;
		bool m_isRegistered = false;
	};
}
