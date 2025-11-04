#pragma once
#include "Graphics/Core/GfxDeviceChild.h"

namespace Ryu::Gfx
{
	class Fence : public DeviceChild
	{
		static constexpr auto TIMEOUT_TIME = 5000;
	public:
		Fence(Device* parent, u64 initialValue, std::string_view name);
		~Fence() override;

		virtual inline void ReleaseObject() override { ComRelease(m_fence); }

		RYU_DISABLE_COPY(Fence)
		RYU_GFX_NATIVE(m_fence)

		void Wait(u64 value);
		void Signal(u64 value);

		bool IsCompleted(u64 value) const;
		u64 GetCompletedValue() const;

	private:
		ComPtr<DX12::Fence> m_fence;
		HANDLE              m_event;
	};
}
