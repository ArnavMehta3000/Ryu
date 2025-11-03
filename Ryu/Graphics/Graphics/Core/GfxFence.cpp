#include "Graphics/Core/GfxFence.h"
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	GfxFence::GfxFence(GfxDevice* parent, u64 initialValue, std::string_view name)
		: GfxDeviceChild(parent)
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
	
	GfxFence::~GfxFence()
	{
		if (m_event)
		{
			::CloseHandle(m_event);
		}
	}
	
	void GfxFence::Wait(u64 value)
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
	
	void GfxFence::Signal(u64 value)
	{
		// Sets the fence to a specific value (CPU-side
		DXCall(m_fence->Signal(value));
	}
	
	bool GfxFence::IsCompleted(u64 value) const
	{
		return GetCompletedValue() >= value;
	}
	
	u64 GfxFence::GetCompletedValue() const
	{
		return m_fence->GetCompletedValue();
	}
}
