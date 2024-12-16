#pragma once
#include "GraphicsDX11/DX11Types.h"
#include "GraphicsRHI/IRenderTarget.h"
#include "GraphicsDX11/DX11Texture2D.h"

namespace Ryu::Graphics::DX11
{
	class DX11Device;
	class DX11Texture2D;

	struct RenderTargetDesc
	{
		u32 Width         = 0;
		u32 Height        = 0;
		u32 SampleCount   = 1;
		u32 SampleQuality = 0;
		Format Format     = Format::R8G8B8A8_UNORM;
		TextureUsage Usage = TextureUsage::RenderTarget;
	};

	class DX11RenderTarget : public IRenderTarget, public IGraphicsNamedObject<DX11::IDX11RenderTarget>
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_renderTarget.Get())

		DX11RenderTarget(const DX11Device* device, const RenderTargetDesc& desc);
		DX11RenderTarget(const DX11Device* device, DX11Texture2D* texture);
		~DX11RenderTarget();

	private:
		void Clear(const f32 clearColor[4]) override;
		Format GetFormat() const override { return m_desc.Format; }
		u32 GetWidth()     const override { return m_desc.Width; }
		u32 GetHeight()    const override { return m_desc.Height; }
		ITexture2D* GetTexture() const override { return m_texture.get(); }

		void CreateRenderTargetView();

	private:
		const DX11Device*              m_device;
		ComPtr<NativeType>             m_renderTarget;
		RenderTargetDesc               m_desc;
		std::unique_ptr<DX11Texture2D> m_texture;
	};
}
