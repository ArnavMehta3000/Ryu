#include "DX12CommandList.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX12/DX12Device.h"
#include <libassert/assert.hpp>
#include <format>

namespace Ryu::Graphics::DX12
{
	static u32 _count = 0;

	DX12CommandList::DX12CommandList(const DX12Device* device, const CommandListDesc& desc)
		: DX12DeviceResource(device)
		, ICommandList(desc)
		, m_fenceEvent(nullptr)
		, m_fenceValue(0)
	{
		RYU_PROFILE_SCOPE();
		Initialize();

		GetDevice()->InitializeResource(this);
		SetName(std::format("Deferred Context {}", _count++));
	}

	DX12CommandList::~DX12CommandList()
	{
		RYU_PROFILE_SCOPE();
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

	void DX12CommandList::WaitForGPU(DWORD milliseconds /* = INFINITE*/) const
	{
		RYU_PROFILE_SCOPE();
		if (m_fence->GetCompletedValue() < m_fenceValue)
		{
			DXCall(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
			::WaitForSingleObject(m_fenceEvent, milliseconds);
		}
	}

	u64 DX12CommandList::Signal() const
	{
		RYU_PROFILE_SCOPE();
		
		const u64 fenceValueForSignal = ++m_fenceValue;
		DXCall(GetDevice()->GetCommandQueue()->Signal(m_fence.Get(), fenceValueForSignal));

		return fenceValueForSignal;
	}

	void DX12CommandList::Flush()
	{
		Signal();
		WaitForGPU();
	}

	void DX12CommandList::ClearRenderTargetView(IRenderTarget* renderTarget, const f32* clearColor)
	{
		LOG_FATAL(RYU_USE_LOG_CATEGORY(DX12CommandList), "DX12CommandList::ClearRenderTargetView not implemented");
	}

	void DX12CommandList::Begin()
	{
		LOG_FATAL(RYU_USE_LOG_CATEGORY(DX12CommandList), "DX12CommandList::Begin not implemented");
	}
	
	void DX12CommandList::End()
	{
		LOG_FATAL(RYU_USE_LOG_CATEGORY(DX12CommandList), "DX12CommandList::End not implemented");
	}
	
	void DX12CommandList::Reset()
	{
		LOG_FATAL(RYU_USE_LOG_CATEGORY(DX12CommandList), "DX12CommandList::Reset not implemented");
	}
	
	void DX12CommandList::Initialize()
	{
		RYU_PROFILE_SCOPE();
		
		DX12Device::NativeType* nativeDevice = *GetDevice();
		DEBUG_ASSERT(nativeDevice);

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
		RYU_TODO("Add support for pipeline state");
		DXCall(nativeDevice->CreateCommandList(0, cmdListType, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_gfxCmdList.ReleaseAndGetAddressOf())));
		DX12_NAME_OBJECT(m_gfxCmdList.Get(), name);

		// Close the command list since it starts in recording state
		DXCall(m_gfxCmdList->Close());

		// Create synchronization objects
		m_fence.Attach(GetDevice()->CreateFence());
		DX12_NAME_OBJECT(m_fence.Get(), L"Graphics Fence");

		m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		ASSERT(m_fenceEvent, "Failed to create fence event!");
	}
}
