#pragma once
#include "Graphics/Core/GfxDeviceChild.h"

namespace Ryu::Gfx
{
	class GfxFence : public GfxDeviceChild
	{
		static constexpr auto TIMEOUT_TIME = 5000;
	public:
		GfxFence(GfxDevice* parent, u64 initialValue, std::string_view name);
		~GfxFence() override;

		virtual inline void ReleaseObject() override { ComRelease(m_fence); }

		RYU_DISABLE_COPY(GfxFence)
		RYU_GFX_NATIVE(m_fence)

		void Wait(u64 value);
		void Signal(u64 value);

		bool IsCompleted(u64 value) const;
		u64 GetCompletedValue() const;

	private:
		ComPtr<DX12::Fence> m_fence;
		HANDLE              m_event;
	};

	class GraphicsFence : public GfxFence
	{
	public:
		GraphicsFence(GfxDevice* parent, u64 initialValue) 
			: GfxFence(parent, initialValue, "Graphics Fence") {}
	};

	class ComputeFence : public GfxFence
	{
	public:
		ComputeFence(GfxDevice* parent, u64 initialValue) 
			: GfxFence(parent, initialValue, "Compute Fence") {}
	};
}
