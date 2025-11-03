#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GfxDevice;

	class GfxDeviceChild
	{
	public:
		GfxDeviceChild()
			: m_device(nullptr)
		{
		}

		explicit GfxDeviceChild(GfxDevice* device);  // Defined in GfxDevice.cpp
		virtual ~GfxDeviceChild();  // Defined in GfxDevice.cpp

		virtual void ReleaseObject() { /* Can be overridden if needed */ }
		[[nodiscard]] inline GfxDevice* GetDevice() const noexcept { return m_device; }

	private:
		GfxDevice* m_device;
	};
}
