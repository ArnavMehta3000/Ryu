#pragma once
#include "GraphicsDX11/DX11Types.h"
#include "GraphicsRHI/ITexture2D.h"

namespace Ryu::Graphics::DX11
{
	class DX11Device;

	class DX11Texture2D : public ITexture2D, public IGraphicsNamedObject<DX11::IDX11Texture2D>
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_texture.Get())

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
		const DX11Device*  m_device;
		Texture2DDesc      m_desc;
	};
}
