#include "DX11CommandList.h"
#include "Common/StandardTypes.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX11/DX11Device.h"
#include "GraphicsDX11/DX11RenderTarget.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX11
{
	static u32 _count = 0;
	
	DX11CommandList::DX11CommandList(const DX11Device* device, const CommandListDesc& desc)
		: ICommandList(desc)
		, m_device(device)
	{
		RYU_PROFILE_SCOPE();
		DX11Device::NativeType* nativeDevice = *m_device;
		
		ComPtr<ID3D11DeviceContext3> baseContext;
		DXCall(nativeDevice->CreateDeferredContext3(0, &baseContext));
		DXCall(baseContext->QueryInterface(IID_PPV_ARGS(m_context.ReleaseAndGetAddressOf())));

		SetName(std::format("Deferred Context {}", _count++));
		m_device->InitializeResource(this);
	}

	DX11CommandList::~DX11CommandList()
	{
		RYU_PROFILE_SCOPE();
		m_cmdList.Reset();
		m_context.Reset();
	}

	void DX11CommandList::Begin()
	{
		RYU_PROFILE_SCOPE();
		m_cmdList.Reset();
	}
	
	void DX11CommandList::End()
	{
		RYU_PROFILE_SCOPE();
		DXCall(m_context->FinishCommandList(FALSE, m_cmdList.ReleaseAndGetAddressOf()));
	}
	
	void DX11CommandList::Reset()
	{
		RYU_PROFILE_SCOPE();
		if (m_cmdList)
		{
			m_cmdList.Reset();
		}
	}

	void DX11CommandList::ClearRenderTargetView(IRenderTarget* renderTarget, const f32* clearColor)
	{
		RYU_PROFILE_SCOPE();
		DEBUG_ASSERT(m_context, "DX11 deferred context is not initialized!");
		
		DX11RenderTarget* dxRenderTarget = static_cast<DX11RenderTarget*>(renderTarget);
		ClearRenderTargetViewImpl(dxRenderTarget, clearColor);
	}
	
	void DX11CommandList::ClearRenderTargetViewImpl(DX11RenderTarget* renderTarget, const f32* clearColor)
	{
		RYU_PROFILE_SCOPE();
		m_context->ClearRenderTargetView(*renderTarget, clearColor);
	}
}
