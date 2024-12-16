#include "DX11Texture2D.h"
#include "GraphicsRHI/Utils/D3DUtil.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX11/DX11Device.h"

namespace Ryu::Graphics::DX11
{
	DX11Texture2D::DX11Texture2D(const DX11Device* device, const Texture2DDesc& desc)
		: m_device(device)
		, m_desc(desc)
	{
		CreateNativeTexture();
	}

	DX11Texture2D::DX11Texture2D(const DX11Device* device, NativeType* texture)
		: m_device(device)
		, m_texture(texture)
	{
		// Build texture description
		D3D11_TEXTURE2D_DESC1 nativeDesc;
		m_texture->GetDesc1(&nativeDesc);

		m_desc.Width         = nativeDesc.Width;
		m_desc.Height        = nativeDesc.Height;
		m_desc.Format        = Utils::GetFormat(nativeDesc.Format);
		m_desc.ArraySize     = nativeDesc.ArraySize;
		m_desc.MipLevels     = nativeDesc.MipLevels;
		m_desc.SampleCount   = nativeDesc.SampleDesc.Count;
		m_desc.SampleQuality = nativeDesc.SampleDesc.Quality;

		// Convert bind flags to usage
		if (nativeDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			m_desc.Usage |= TextureUsage::RenderTarget;
		}
		if (nativeDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			m_desc.Usage |= TextureUsage::DepthStencil;
		}
		if (nativeDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			m_desc.Usage |= TextureUsage::ShaderResource;
		}
		if (nativeDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			m_desc.Usage |= TextureUsage::UnorderedAccess;
		}
	}

	DX11Texture2D::~DX11Texture2D()
	{
		m_texture.Reset();
	}
	
	void DX11Texture2D::CreateNativeTexture()
	{
		CD3D11_TEXTURE2D_DESC1 texDesc(
			Utils::GetFormat(m_desc.Format),
			m_desc.Width,
			m_desc.Height,
			m_desc.ArraySize,
			m_desc.MipLevels,
			0, // Bind flags
			D3D11_USAGE_DEFAULT,
			0,
			m_desc.SampleCount,
			m_desc.SampleQuality,
			0);

		if (Ryu::IsEnumMaskBitSet(m_desc.Usage, TextureUsage::RenderTarget))
			texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		if (Ryu::IsEnumMaskBitSet(m_desc.Usage, TextureUsage::DepthStencil))
			texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		if (Ryu::IsEnumMaskBitSet(m_desc.Usage, TextureUsage::ShaderResource))
			texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (Ryu::IsEnumMaskBitSet(m_desc.Usage, TextureUsage::UnorderedAccess))
			texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

		DX11Device::NativeType* nativeDevice = *m_device;
		DXCall(nativeDevice->CreateTexture2D1(&texDesc, nullptr, m_texture.ReleaseAndGetAddressOf()));
		SetName("DX11 Texture 2D");
	}
}
