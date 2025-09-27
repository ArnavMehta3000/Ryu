#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	GfxCommandList::GfxCommandList(GfxDevice* device, CommandListType type, std::string_view name)
		: m_device(device)
		, m_type(type)
		, m_cmdQueue(device->GetCommandQueue(type))
	{
		D3D12_COMMAND_LIST_TYPE nativeType = DX12::ToNative(type);

		DX12::Device* dx12Device = m_device->GetDevice();

		DXCallEx(dx12Device->CreateCommandAllocator(nativeType, IID_PPV_ARGS(&m_cmdAllocator)), device->GetDevice());
		DXCallEx(dx12Device->CreateCommandList(0, nativeType, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList)), device->GetDevice());

		SetName(name.data());
		m_cmdList->Close();
	}
	
	void GfxCommandList::ResetAllocator()
	{
		m_cmdAllocator->Reset();
	}
	
	void GfxCommandList::Begin()
	{
		m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);
		// Reset state
	}
	
	void GfxCommandList::End()
	{
		// Flush Barriers
		m_cmdList->Close();
	}
	
	void GfxCommandList::Wait(GfxFence& fence, u64 value)
	{
		m_pendingWaits.emplace_back(fence, value);
	}
	
	void GfxCommandList::Signal(GfxFence& fence, u64 value)
	{
		m_pendingSignals.emplace_back(fence, value);
	}
	
	void GfxCommandList::WaitAll()
	{
		for (auto& [fence, value] : m_pendingWaits)
		{
			m_cmdQueue.Wait(fence, value);
		}
		m_pendingWaits.clear();
	}
	
	void GfxCommandList::Submit()
	{
		WaitAll();
		std::array lists{ this };
		m_cmdQueue.ExecuteCommandLists(lists);
		SignalAll();
	}
	
	void GfxCommandList::SignalAll()
	{
		for (auto& [fence, value] : m_pendingSignals)
		{
			m_cmdQueue.Signal(fence, value);
		}
		m_pendingSignals.clear();
	}
}
