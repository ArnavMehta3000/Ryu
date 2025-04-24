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
		u64 Signal(CommandQueue* cmdQueue);
		u64 Signal(u64 fenceValue);
		// Stall CPU until fence value is signaled on the GPU
		void CPUWait(u64 fenceValue);
		void CPUWait();
		// Returns true if the fence has reached this value or higher
		bool IsComplete(u64 fenceValue);

		inline NODISCARD u64 GetCurrentValue() const { return m_currentValue; }
		inline NODISCARD u64 GetLastSignaledValue() const { return m_lastSignaledValue; }
		inline NODISCARD ComPtr<DX12::Fence> GetFence() const { return m_fence; }

	private:
		ComPtr<DX12::Fence> m_fence;
		std::mutex m_fenceWaitMutex;
		HANDLE m_completeEvent;
		u64 m_currentValue;
		u64 m_lastSignaledValue;
		u64 m_lastCompletedValue;
	};

	class SyncPoint
	{
	public:
		explicit SyncPoint(Fence* fence, u64 fenceValue);

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
