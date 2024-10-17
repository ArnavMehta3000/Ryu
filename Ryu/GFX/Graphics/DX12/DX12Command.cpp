#include "DX12Command.h"
#include <libassert/assert.hpp>
#include <array>

namespace Ryu::Graphics::DX12::Core
{
	DX12Command::DX12Command(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type)
	{
		DEBUG_ASSERT(device);

		HRESULT hr{ S_OK };
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = type;
		desc.NodeMask = 0;

		DXCall(hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)));
		if (FAILED(hr)) Release();
		DX12_NAME_OBJECT(m_cmdQueue, 
			type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command Queue" : 
			type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Queue" : L"Command Queue");

		for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			CommandFrame& frame = m_cmdFrames[i];
			DXCall(hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&frame.Allocator)));
			if (FAILED(hr)) Release();

			DX12_NAME_OBJECT(frame.Allocator, std::format(L"{}[{}]",
				type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command Allocator" :
				type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Allocator" : L"Command Allocator", i));
		}

		DXCall(hr = device->CreateCommandList(0, type, m_cmdFrames[0].Allocator, nullptr, IID_PPV_ARGS(&m_cmdList)));
		if (FAILED(hr)) Release();

		// Close command list
		DXCall(m_cmdList->Close());

		DXCall(hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

		m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		ASSERT(m_fenceEvent);
	}

	DX12Command::~DX12Command()
	{
		DEBUG_ASSERT(!m_cmdQueue && !m_cmdList && !m_fence);
	}

	void DX12Command::Flush()
	{
		for (auto& frame : m_cmdFrames)
		{
			frame.Wait(m_fenceEvent, m_fence);
		}
		m_frameIndex = 0;
	}

	void DX12Command::Release()
	{
		Flush();

		Graphics::Release(m_fence);
		m_fenceValue = 0;

		::CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;
		
		Graphics::Release(m_cmdQueue);
		Graphics::Release(m_cmdList);

		for (auto& frame : m_cmdFrames)
		{
			frame.Release();
		}
	}

	void DX12Command::BeginFrame()
	{
		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.Wait(m_fenceEvent, m_fence);

		DXCall(frame.Allocator->Reset());
		DXCall(m_cmdList->Reset(frame.Allocator, nullptr));
	}

	void DX12Command::EndFrame()
	{
		DXCall(m_cmdList->Close());

		const std::array cmdLists = { static_cast<ID3D12CommandList*>(m_cmdList) };
		m_cmdQueue->ExecuteCommandLists((u32)cmdLists.size(), cmdLists.data());

		// If frame's fence value is < signalled fence value means the GPU is done
		// processing the frame and it can be displayed -> reuse to record new commands

		u64& fv = m_fenceValue;
		++fv;
		CommandFrame& frame = m_cmdFrames[m_frameIndex];
		frame.FenceValue = fv;
		m_cmdQueue->Signal(m_fence, fv);

		m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;
	}

	void DX12Command::CommandFrame::Wait(HANDLE fenceEvent, ID3D12Fence1* fence) const noexcept
	{
		DEBUG_ASSERT(fence && fenceEvent);

		// If current fence value is still less than the "FenceValue"
		// Then we know that the GPU has not finished processing the frame (executing the command list)
		// since it has not reached the Signaled state
		if (fence->GetCompletedValue() < FenceValue)
		{
			DXCall(fence->SetEventOnCompletion(FenceValue, fenceEvent));

			// Wait for the event to be signaled
			::WaitForSingleObject(fenceEvent, INFINITE);
		}
	}
}
