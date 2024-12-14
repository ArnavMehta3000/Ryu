#include "DX11CommandList.h"
#include "Common/StandardTypes.h"
#include "Graphics/Shared/Logging.h"
#include "Graphics/DX11/DX11Device.h"

namespace Ryu::Graphics
{
	DX11CommandList::DX11CommandList(const DX11Device& device, const CommandListDesc& desc)
		: ICommandList(desc)
		, m_device(device)
	{
		static u32 _count = 0;
		DX11Device::NativeType* nativeDevice = m_device;
		ComPtr<ID3D11DeviceContext3> baseContext;
		
		DXCall(nativeDevice->CreateDeferredContext3(0, &baseContext));
		DXCall(baseContext->QueryInterface(IID_PPV_ARGS(m_deferredContext.ReleaseAndGetAddressOf())));
		DX11_NAME_OBJECT(m_deferredContext.Get(), std::format("Deferred Context {}", _count++));
	}

	DX11CommandList::~DX11CommandList()
	{
		m_cmdList.Reset();
		m_deferredContext.Reset();
	}
	
	NativeObjectType DX11CommandList::GetNativeObject() const
	{
		return m_cmdList.Get();
	}
	
	void DX11CommandList::Begin()
	{
		m_cmdList.Reset();
	}
	
	void DX11CommandList::End()
	{
		DXCall(m_deferredContext->FinishCommandList(FALSE, m_cmdList.ReleaseAndGetAddressOf()));
	}
	
	void DX11CommandList::Reset()
	{
		if (m_cmdList)
		{
			m_cmdList.Reset();
		}
	}
}
