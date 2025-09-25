#include "Graphics/Core/GfxFence.h"
#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	GfxFence::GfxFence() = default;
	
	GfxFence::~GfxFence()
	{
		if (m_event)
		{
			::CloseHandle(m_event);
		}
	}

	void GfxFence::Create(GfxDevice* device, std::string_view name)
	{
		DX12::Device* dx12Device = device->GetDevice();
		DXCallEx(dx12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), dx12Device);
		
		SetName(name.data());
		m_event = ::CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
	}
	
	void GfxFence::Wait(u64 value)
	{
		RYU_ASSERT(m_fence && m_event, "Fence and event must be created before waiting.");

		if (!IsCompleted(value))
		{
			m_fence->SetEventOnCompletion(value, m_event);
			::WaitForSingleObjectEx(m_event, INFINITE, false);
		}
	}
	
	void GfxFence::Signal(u64 value)
	{
		RYU_ASSERT(m_fence, "Fence must be created before signaling.");
		m_fence->Signal(value);
	}
	
	bool GfxFence::IsCompleted(u64 value) const
	{
		return GetCompletedValue() >= value;
	}
	
	u64 GfxFence::GetCompletedValue() const
	{
		RYU_ASSERT(m_fence, "Fence must be created before checking completion.");
		return m_fence->GetCompletedValue();
	}
}
