#include "Graphics/Fence.h"
#include "Graphics/Device.h"

namespace Ryu::Gfx
{
	Fence::Fence(Device* parent, const char* name, u64 fenceValue)
		: DeviceObject(parent)
		, m_currentValue(fenceValue + 1)
		, m_lastSignaledValue(0)
		, m_lastCompletedValue(fenceValue)
	{
		DXCallEx(parent->GetDevice()->CreateFence(m_lastCompletedValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())), parent->GetDevice().Get());
		DX12::SetObjectName(m_fence.Get(), name);
		m_completeEvent = ::CreateEventExA(nullptr, "Fence Event", 0, EVENT_ALL_ACCESS);
	}
	
	Fence::~Fence()
	{
		::CloseHandle(m_completeEvent);
	}
	
	u64 Fence::Signal(CommandQueue* cmdQueue)
	{
		cmdQueue->GetCommandQueue()->Signal(m_fence.Get(), m_currentValue);
		m_lastSignaledValue = m_currentValue;
		m_currentValue++;
		return m_lastSignaledValue;
	}
	
	u64 Fence::Signal(u64 fenceValue)
	{
		m_lastSignaledValue = fenceValue;
		m_lastCompletedValue = fenceValue;
		m_currentValue++;
		return m_lastSignaledValue;
	}
	
	void Fence::CPUWait(u64 fenceValue)
	{
		if (IsComplete(fenceValue))
		{
			return;
		}

		std::lock_guard<std::mutex> lockGuard(m_fenceWaitMutex);

		m_fence->SetEventOnCompletion(fenceValue, m_completeEvent);
		DWORD result = ::WaitForSingleObject(m_completeEvent, INFINITE);

		m_lastCompletedValue = m_fence->GetCompletedValue();
	}
	
	void Fence::CPUWait()
	{
		CPUWait(m_lastSignaledValue);
	}
	
	bool Fence::IsComplete(u64 fenceValue)
	{
		if (fenceValue <= m_lastCompletedValue)
		{
			return true;
		}

		m_lastCompletedValue = std::max(m_lastCompletedValue, m_fence->GetCompletedValue());
		return fenceValue <= m_lastCompletedValue;
	}
	
	SyncPoint::SyncPoint(Fence* fence, u64 fenceValue)
		: m_fence(fence)
		, m_fenceValue(fenceValue)
	{
	}

	void SyncPoint::Wait() const
	{
		m_fence->CPUWait(m_fenceValue);
	}
	
	bool SyncPoint::IsComplete() const
	{
		return m_fence->IsComplete(m_fenceValue);
	}
}
