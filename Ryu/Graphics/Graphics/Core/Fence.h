#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	class Fence : public DeviceObject<Fence>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		Fence() = default;
		Fence(std::weak_ptr<Device> parent, u64 initialValue = 0, std::string_view name = "");
		~Fence();

		// Set a fence value from the GPU
		void SignalGPU(CommandCtx* cmdContext, u64 value);

		// Set a fence value from the CPU
		void SignalCPU(u64 value);

		// Block the GPU until a fence value is reached
		void WaitGPU(CommandCtx* cmdContext, u64 value) const;

		// Block the CPU until a fence value is reached
		void WaitCPU(u64 value) const;

		// Non-blocking check to see if a fence value has been reached
		bool IsCompleted(u64 value) const;
		
		// Get the current completed value
		u64 GetCompletedValue() const;
		
		// Reset the fence state
		void ResetFence();
		
		inline NODISCARD u64 GetNextValue() { return ++m_currentValue; }
		inline NODISCARD u64 GetCurrentValue() const { return m_currentValue; }
		inline NODISCARD DX12::Fence* GetFence() const noexcept { return m_fence.Get(); }

	private:
		void OnConstruct(u64 initialValue, std::string_view name);

	private:
		ComPtr<DX12::Fence> m_fence;
		HANDLE              m_handle{ nullptr };
		u64                 m_currentValue{ 0 };
	};
}
