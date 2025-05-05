#include "Graphics/Texture.h"
#include "Graphics/Device.h"

namespace Ryu::Gfx
{
	Texture::Texture(Device* parent, const TextureDesc& desc)
		: DeviceObject(parent)
	{
		DX12::Device* const device = GetParent()->GetDevice();

		m_srv = GetParent()->GetSRVHeap()->Allocate();
		device->CreateShaderResourceView(m_resource.Get(), nullptr, m_srv.CPUHandle);
	}
	
	Texture::~Texture()
	{
		if (m_resource)
		{
			GetParent()->DeferReleaseObject(m_resource.Detach());
		}
	}
}
