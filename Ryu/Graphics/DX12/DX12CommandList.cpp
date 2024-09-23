#include "DX12CommandList.h"
#include <Graphics/DX12/DX12Device.h>

namespace Ryu::Graphics
{
	DX12CommandList::~DX12CommandList()
	{
		Release();
	}

	void DX12CommandList::Create(const DX12Device& device, D3D12_COMMAND_LIST_TYPE type)
	{
		RYU_GFX_ASSERT(device, "Trying to create DX12CommandList with invalid DX12Device");
		
		HRESULT hr = S_OK;
		RYU_GFX_ASSERTHR(hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(m_allocator.ReleaseAndGetAddressOf())), "Failed to create DX12CommandAllocator");
		RYU_GFX_ASSERTHR(hr = device->CreateCommandList(0, type, m_allocator.Get(), nullptr, IID_PPV_ARGS(ReleaseAndGetAddressOf())), "Failed to create DX12CommandList");
	
		Get()->SetName(L"DX12CommandList");
		m_allocator->SetName(L"DX12CommandAllocator");
	}

	void DX12CommandList::Release()
	{
		if (Get())
		{
			Reset();
		}

		if (m_allocator.Get())
		{
			m_allocator.Reset();
		}
	}
}
