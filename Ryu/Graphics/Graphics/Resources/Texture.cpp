#include "Graphics/Resources/Texture.h"
#include "Graphics/Core/Device.h"

namespace Ryu::Gfx
{
	Texture::Texture(std::weak_ptr<Device> parent, const TextureDesc& desc)
		: DeviceObject(parent)
	{
		if (auto parentDevice = GetParent())
		{
			DX12::Device* const device = parentDevice->GetDevice();

			m_srv = parentDevice->GetSRVHeap()->Allocate();
			device->CreateShaderResourceView(m_resource.Get(), nullptr, m_srv.CPUHandle);
		}
	}
	
	Texture::~Texture()
	{
		if (m_resource)
		{
			if (auto parent = GetParent())
			{
				parent->DeferReleaseObject(m_resource.Detach());
			}
		}
	}
}
