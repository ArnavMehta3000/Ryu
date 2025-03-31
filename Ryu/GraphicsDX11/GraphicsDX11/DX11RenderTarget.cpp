#include "DX11RenderTarget.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/Utils/D3DUtil.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX11/DX11Device.h"

namespace Ryu::Graphics::DX11
{
	DX11RenderTarget::DX11RenderTarget(const DX11Device* device, const RenderTargetDesc& desc)
		: m_device(device)
		, m_desc(desc)
	{
		RYU_PROFILE_SCOPE();
		Texture2DDesc texDesc;
		texDesc.Width         = desc.Width;
		texDesc.Height        = desc.Height;
		texDesc.Format        = desc.Format;
		texDesc.ArraySize     = 1;
		texDesc.MipLevels     = 1;
		texDesc.SampleCount   = desc.SampleCount;
		texDesc.SampleQuality = desc.SampleQuality;
		texDesc.Usage         = TextureUsage::RenderTarget;

		if (Ryu::IsEnumMaskBitSet(desc.Usage, TextureUsage::ShaderResource))
		{
			texDesc.Usage |= TextureUsage::ShaderResource;
		}

		m_texture = std::make_unique<DX11Texture2D>(device, texDesc);
		m_device->InitializeResource(m_texture.get());

		CreateRenderTargetView();
	}
	
	DX11RenderTarget::DX11RenderTarget(const DX11Device* device, DX11Texture2D* texture)
		: m_device(device)
	{
		RYU_PROFILE_SCOPE();
		DX11Texture2D::NativeType* nativeTexture = *texture;
		m_texture = std::make_unique<DX11Texture2D>(device, nativeTexture);

		CreateRenderTargetView();
	}
	
	DX11RenderTarget::~DX11RenderTarget()
	{
		RYU_PROFILE_SCOPE();
		m_renderTarget.Reset();
		m_texture.reset();
	}
	
	void DX11RenderTarget::CreateRenderTargetView()
	{
		RYU_PROFILE_SCOPE();
		D3D11_RENDER_TARGET_VIEW_DESC1 rtvDesc{};
		rtvDesc.Format             = Utils::GetFormat(m_desc.Format);
		rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		DX11Device::NativeType* nativeDevice = *m_device;
		DXCall(nativeDevice->CreateRenderTargetView1(*m_texture, &rtvDesc, m_renderTarget.ReleaseAndGetAddressOf()));
	}
}
