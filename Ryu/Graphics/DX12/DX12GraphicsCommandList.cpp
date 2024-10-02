#include "DX12GraphicsCommandList.h"
#include <Graphics/DX12/DX12Device.h>

namespace Ryu::Graphics
{
	CreateResult<DX12GraphicsCommandList> DX12GraphicsCommandList::Create(const DX12Device& device, D3D12_COMMAND_LIST_TYPE type)
	{
		RYU_GFX_ASSERT(device, "Trying to create DX12GraphicsCommandList with invalid DX12Device");
		
		DX12GraphicsCommandList outCommandList;

		HRESULT hr{ S_OK };
		
		RYU_GFX_ASSERTHR(hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(outCommandList.m_allocator.ReleaseAndGetAddressOf())), "Failed to create DX12CommandAllocator");
		
		if (FAILED(hr = device->CreateCommandList(0, type, outCommandList.m_allocator.Get(), nullptr, IID_PPV_ARGS(outCommandList.ReleaseAndGetAddressOf()))))
		{
			return std::unexpected(hr);
		}
	
		outCommandList->SetName(L"DX12GraphicsCommandList");
		outCommandList.m_allocator->SetName(L"DX12CommandAllocator");

		outCommandList->Close();
		RYU_GFX_DEBUG("Created DX12GraphicsCommandList");
		return outCommandList;
	}
	void DX12GraphicsCommandList::Release()
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

	void DX12GraphicsCommandList::ResetCommandList()
	{
		RYU_GFX_ASSERTHR(m_allocator->Reset(), "Failed to reset DX12CommandAllocator");
		RYU_GFX_ASSERTHR(Get()->Reset(m_allocator.Get(), nullptr), "Failed to reset DX12GraphicsCommandList");
	}
}
