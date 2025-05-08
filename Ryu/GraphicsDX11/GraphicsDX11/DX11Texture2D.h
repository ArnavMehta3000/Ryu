#pragma once
#include "GraphicsDX11/DX11Device.h"
#include "GraphicsRHI/ITexture2D.h"

namespace Ryu::Graphics::DX11
{
	class DX11Device;

	class DX11Texture2D 
		: public DX11DeviceResource
		, public ITexture2D
		, public IGraphicsRHIObject<DX11::IDX11Texture2D>
	{
	public:
		RYU_GFX_DECLARE_NATIVE_TYPE_OP(m_texture.Get())

		DX11Texture2D(const DX11Device* device, const Texture2DDesc& desc);
		DX11Texture2D(const DX11Device* device, NativeType* texture);
		~DX11Texture2D();

	private:
		Format GetFormat()      const override { return m_desc.Format; }
		TextureUsage GetUsage() const override { return m_desc.Usage;  }
		u32 GetWidth()          const override { return m_desc.Width;  }
		u32 GetHeight()         const override { return m_desc.Height; }

		void CreateNativeTexture();

	private:
		ComPtr<NativeType> m_texture;
		Texture2DDesc      m_desc;
	};
}
