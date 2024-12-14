#include "DX12CommandList.h"
#include "Graphics/Shared/Logging.h"
#include "Graphics/DX12/DX12Device.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics
{
	DX12CommandList::DX12CommandList(const DX12Device& device, const CommandListDesc& desc)
		: ICommandList(desc)
		, m_device(device)
		, m_fenceEvent(nullptr)
		, m_fenceValue(0)
	{
		Initialize();		
	}

	DX12CommandList::~DX12CommandList()
	{
		if (m_fence && m_fenceEvent)
		{
			Signal();
			WaitForGPU();
			::CloseHandle(m_fenceEvent);
		}

		m_gfxCmdList.Reset();
		m_fence.Reset();
		m_cmdAllocator.Reset();
		m_fenceEvent = nullptr;
	}
	
	NativeObjectType DX12CommandList::GetNativeObject() const
	{
		return m_gfxCmdList.Get();
	}

	void DX12CommandList::WaitForGPU() const
	{
		if (m_fence->GetCompletedValue() < m_fenceValue)
		{
			DXCall(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
			::WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}

	void DX12CommandList::Signal() const
	{
		m_fenceValue++;
		DXCall(m_device.GetCommandQueue()->Signal(m_fence.Get(), m_fenceValue));
	}

	void DX12CommandList::Begin()
	{
	}
	
	void DX12CommandList::End()
	{
	}
	
	void DX12CommandList::Reset()
	{
	}
	
	void DX12CommandList::Initialize()
	{
		DX12Device::NativeType* nativeDevice = m_device;
		if (!nativeDevice)
		{
			return;
		}

		std::wstring name = L"Graphics Command List";

		D3D12_COMMAND_LIST_TYPE cmdListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		switch (m_desc.Type)
		{
		case CommandListType::Compute:
			name = L"Compute Command List";
			cmdListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case CommandListType::Copy:
			name = L"Copy Command List";
			cmdListType = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		}

		// Create command allocator
		DXCall(nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)));
		DX12_NAME_OBJECT(m_cmdAllocator.Get(), L"Graphics Command Allocator");

		// Create command list
		DXCall(nativeDevice->CreateCommandList(0, cmdListType, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_gfxCmdList.ReleaseAndGetAddressOf())));
		DX12_NAME_OBJECT(m_gfxCmdList.Get(), name);

		// Close the command list since it starts in recording state
		DXCall(m_gfxCmdList->Close());

		// Create synchronization objects
		DXCall(nativeDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
		DX12_NAME_OBJECT(m_fence.Get(), L"Graphics Fence");

		m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		ASSERT(m_fenceEvent, "Failed to create fence event!");
	}
}
