#pragma once
#include "GraphicsDX11/DX11DeviceResource.h"
#include "GraphicsRHI/ICommandList.h"

namespace Ryu::Graphics::DX11
{
	class DX11Device;
	class DX11RenderTarget;

	class DX11CommandList 
		: public DX11DeviceResource
		, public ICommandList
		, public IGraphicsRHIObject<DX11::ID3D11CommandList>
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_cmdList.Get())

		DX11CommandList(const DX11Device* device, const CommandListDesc& desc);
		~DX11CommandList();

		inline DX11::IDX11DeviceContext* GetContext() const { return m_context.Get(); }

		void ClearRenderTargetView(IRenderTarget* renderTarget, const f32* clearColor) override;
		
	private:
		void Begin() override;
		void End() override;
		void Reset() override;

		void ClearRenderTargetViewImpl(DX11RenderTarget* renderTarget, const f32* clearColor);

	private:
		ComPtr<NativeType>               m_cmdList;
		ComPtr<DX11::IDX11DeviceContext> m_context;
	};
}
