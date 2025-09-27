#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	GfxTexture::GfxTexture(GfxDevice* device, const GfxTexture::Desc& desc, DX12::Resource* backBuffer)
		: m_device(device)
		, m_desc(desc)
		, m_resource(backBuffer)
		, m_isBackBuffer(true)
	{
	}
	
	GfxTexture::~GfxTexture()
	{
		RYU_TODO("Check for mapped data");
		
		if (!m_isBackBuffer)
		{
			RYU_TODO("Add to release queue in device")
		}
	}
	
	u64 GfxTexture::GetGPUAddress() const noexcept
	{
		return m_resource->GetGPUVirtualAddress();
	}
}
