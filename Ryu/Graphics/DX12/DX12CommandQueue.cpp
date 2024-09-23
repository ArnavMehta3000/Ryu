#include "DX12CommandQueue.h"

namespace Ryu::Graphics
{
	DX12CommandQueue::~DX12CommandQueue()
	{
		Release();
	}

	void DX12CommandQueue::Create(ID3D12Device6* device, D3D12_COMMAND_QUEUE_PRIORITY priority, D3D12_COMMAND_LIST_TYPE type)
	{
		RYU_GFX_ASSERT(device, "Trying to create DX12CommandQueue with nullptr ID3D12Device6");

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.NodeMask = 0;
		desc.Type     = type;
		desc.Priority = priority;
		desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;

		HRESULT hr = S_OK;
		RYU_GFX_ASSERTHR(hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(ReleaseAndGetAddressOf())), "Failed to create DX12CommandQueue");
		RYU_GFX_ASSERTHR(hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), "Failed to create DX12Fence");

		Get()->SetName(L"DX12CommandQueue");
		m_fence->SetName(L"DX12Fence");
	}

	void DX12CommandQueue::Release()
	{
		if (Get())
		{
			Reset();
		}

		if (m_fence.Get())
		{
			m_fence.Reset();
		}
	}
}
