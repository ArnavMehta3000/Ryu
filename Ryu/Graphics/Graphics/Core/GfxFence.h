#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GFxDevice;

	class GfxFence
	{
	public:
		RYU_GFX_NATIVE(m_fence)

		GfxFence();
		~GfxFence();

		void Create(GfxDevice* device, std::string_view name);
		
		void Wait(u64 value);
		void Signal(u64 value);

		bool IsCompleted(u64 value) const;
		u64 GetCompletedValue() const;

	private:
		ComPtr<DX12::Fence> m_fence;
		HANDLE m_event = nullptr;
	};
}
