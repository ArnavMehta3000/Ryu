#include "DX12CommandQueue.h"
#include <Graphics/DX12/DX12Device.h>

namespace Ryu::Graphics
{
	DX12CommandQueue::DX12CommandQueue(InterfaceType* ptr)
		: ComPtr(ptr)
	{
	}

	DX12CommandQueue::~DX12CommandQueue()
	{
		Release();
	}

	void DX12CommandQueue::Init(const DX12Device& device)
	{
		HRESULT hr{ S_OK };
		RYU_GFX_ASSERTHR(hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), "Failed to create DX12Fence");

		m_fence->SetName(L"DX12Fence");
	}

	CreateResult<DX12CommandQueue::InterfaceType*> DX12CommandQueue::Create(const DX12Device& device, D3D12_COMMAND_QUEUE_PRIORITY priority, D3D12_COMMAND_LIST_TYPE type)
	{
		RYU_GFX_ASSERT(device, "Trying to create DX12CommandQueue with invalid DX12Device");
		InterfaceType* outCommandQueue = nullptr;

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
		return outCommandQueue;
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
