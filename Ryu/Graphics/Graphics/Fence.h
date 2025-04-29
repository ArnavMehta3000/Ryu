#pragma once
#include "Graphics/DeviceResource.h"
#include <mutex>

namespace Ryu::Gfx
{
	class Fence : public DeviceObject
	{
	public:
		explicit Fence(Device* parent, const char* name, u64 fenceValue = 0);
		~Fence();

		// Signals on the GPU timeline, increments the next value and return the signaled fence value
		u64 Signal(CmdQueue* cmdQueue);
		u64 Signal(u64 fenceValue);
		// Stall CPU until fence value is signaled on the GPU
		void CPUWait(u64 fenceValue);
		void CPUWait();
		// Returns true if the fence has reached this value or higher
		bool IsComplete(u64 fenceValue);

		inline NODISCARD u64 GetCurrentValue() const noexcept { return m_currentValue; }
		inline NODISCARD u64 GetLastSignaledValue() const noexcept { return m_lastSignaledValue; }
		inline NODISCARD DX12::Fence* GetFence() const noexcept { return m_fence.Get(); }

	private:
		ComPtr<DX12::Fence> m_fence;
		std::mutex          m_fenceWaitMutex;
		HANDLE              m_completeEvent;
		u64                 m_currentValue;
		u64                 m_lastSignaledValue;
		u64                 m_lastCompletedValue;
	};

	class SyncPoint
	{
	public:
		SyncPoint() = default;
		SyncPoint(Fence* fence, u64 fenceValue);

		void Wait() const;
		bool IsComplete() const;
		inline NODISCARD u64 GetFenceValue() const { return m_fenceValue; }
		inline NODISCARD Fence* GetFence() const { return m_fence; }
		inline NODISCARD bool IsValid() const { return m_fence != nullptr; }
		operator bool() const { return IsValid(); }

	private:
		Fence* m_fence = nullptr;
		u64 m_fenceValue;
	};
}
