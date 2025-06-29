#include "Graphics/Core/Fence.h"
#include "Graphics/Core/Device.h"
#include "Graphics/Core/CommandContext.h"
#include "Profiling/Profiling.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
	Fence::Fence(std::weak_ptr<Device> parent, u64 initialValue, std::string_view name)
		: DeviceObject(parent)
	{
		OnConstruct(initialValue, name);
	}

	Fence::~Fence()
	{
		m_fence.Reset();
		::CloseHandle(m_handle);
	}
	
	void Fence::OnConstruct(u64 initialValue, std::string_view name)
	{
		RYU_PROFILE_SCOPE();
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();
			DXCallEx(device->CreateFence(initialValue, DX12::ToNative(FenceFlag::None), IID_PPV_ARGS(&m_fence)), device);

			if (!name.empty())
			{
				DX12::SetObjectName(m_fence.Get(), name.data());
			}

			m_handle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			RYU_ASSERT(m_handle, "Failed to create fence event");
		}
		else
		{
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(GFXFence), "Trying to construct graphics fence. Parent device is null");
		}
	}

	void Fence::SignalGPU(CommandCtx* cmdContext, u64 value)
	{
		RYU_PROFILE_SCOPE();
		DX12::CommandQueue* const cmdQueue = cmdContext ? cmdContext->GetCommandQueue() : nullptr;
		if (cmdQueue && m_fence)
		{
			cmdQueue->Signal(m_fence.Get(), value);
			m_currentValue = value;
		}
	}
	
	void Fence::SignalCPU(u64 value)
	{
		if (m_fence)
		{
			DXCall(m_fence->Signal(value));
			m_currentValue = value;
		}
	}

	void Fence::WaitGPU(CommandCtx* cmdContext, u64 value) const
	{
		RYU_PROFILE_SCOPE();
		DX12::CommandQueue* const cmdQueue = cmdContext ? cmdContext->GetCommandQueue() : nullptr;
		if (/*!IsCompleted(value) &&*/ cmdQueue && m_fence)
		{
			cmdQueue->Wait(m_fence.Get(), value);
		}
	}
	
	void Fence::WaitCPU(u64 value) const
	{
		RYU_PROFILE_SCOPE();
		if (!IsCompleted(value) && m_fence)
		{
			DXCall(m_fence->SetEventOnCompletion(value, m_handle));
			::WaitForSingleObjectEx(m_handle, INFINITE, false);
		}
	}
	
	bool Fence::IsCompleted(u64 value) const
	{
		return m_fence ? m_fence->GetCompletedValue() >= value : false;
	}
	
	u64 Fence::GetCompletedValue() const
	{
		return m_fence ? m_fence->GetCompletedValue() : 0;
	}
	
	void Fence::ResetFence()
	{
		if (m_fence)
		{
			m_fence->Signal(0);
			m_currentValue = 0;
		}
	}
}
