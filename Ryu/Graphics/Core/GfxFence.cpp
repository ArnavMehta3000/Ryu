#include "Graphics/Core/GfxFence.h"
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	Fence::Fence(Device* parent, u64 initialValue, std::string_view name)
		: DeviceChild(parent)
		, m_event(nullptr)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		DXCall(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		DX12::SetObjectName(m_fence.Get(), name.data());

		m_event = ::CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		if (!m_event)
		{
			DXCall(::HRESULT_FROM_WIN32(::GetLastError()));
		}
	}
	
	Fence::~Fence()
	{
		if (m_event)
		{
			::CloseHandle(m_event);
		}
	}
	
	void Fence::Wait(u64 value)
	{
		if (!IsCompleted(value))
		{
			m_fence->SetEventOnCompletion(value, m_event);
			if (::WaitForSingleObjectEx(m_event, TIMEOUT_TIME, FALSE) == WAIT_TIMEOUT)
			{
				RYU_LOG_WARN("Wait for GPU timed out!");
			}
		}
	}
	
	void Fence::Signal(u64 value)
	{
		// Sets the fence to a specific value (CPU-side
		DXCall(m_fence->Signal(value));
	}
	
	bool Fence::IsCompleted(u64 value) const
	{
		return GetCompletedValue() >= value;
	}
	
	u64 Fence::GetCompletedValue() const
	{
		return m_fence->GetCompletedValue();
	}
}
