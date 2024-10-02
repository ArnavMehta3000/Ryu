#include "DX12CommandQueue.h"
#include <Graphics/DX12/DX12Device.h>

namespace Ryu::Graphics
{
	CreateResult<DX12CommandQueue> DX12CommandQueue::Create(const DX12Device& device, D3D12_COMMAND_QUEUE_PRIORITY priority, D3D12_COMMAND_LIST_TYPE type)
	{
		RYU_GFX_ASSERT(device, "Trying to create DX12CommandQueue with invalid DX12Device");
		DX12CommandQueue outCommandQueue;

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.NodeMask  = 0;
		desc.Type      = type;
		desc.Priority  = priority;
		desc.Flags     = D3D12_COMMAND_QUEUE_FLAG_NONE;

		HRESULT hr{ S_OK };
		if (FAILED(hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&outCommandQueue))))
		{
			return std::unexpected(hr);
		}
		
		outCommandQueue->SetName(L"DX12CommandQueue");
		RYU_GFX_DEBUG("Created DX12CommandQueue");
		return outCommandQueue;
	}

	void DX12CommandQueue::Release()
	{
		if (Get())
		{
			Reset();
		}
	}
}
