#pragma once
#include "Graphics/Core/GfxDeviceChild.h"

namespace Ryu::Gfx
{
	class Resource : public DeviceChild
	{
	public:
		Resource(Device* parent) : DeviceChild(parent) {}
		virtual ~Resource() = default;

		virtual inline void ReleaseObject() override { ComRelease(m_resource); }

		RYU_GFX_NATIVE(m_resource);
	
	protected:
		ComPtr<DX12::Resource> m_resource;
	};
}
