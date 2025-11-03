#pragma once
#include "Graphics/Core/GfxDeviceChild.h"
#include <span>

namespace Ryu::Gfx
{
	class GfxFence;
	class GfxCommandList;

	class GfxCommandQueue : public GfxDeviceChild
	{
	public:
		GfxCommandQueue(GfxDevice* parent, D3D12_COMMAND_LIST_TYPE type, std::string_view name);
		virtual ~GfxCommandQueue() override = default;

		virtual inline void ReleaseObject() override { ComRelease(m_cmdQueue); }

		RYU_GFX_NATIVE(m_cmdQueue)

		void Signal(const GfxFence& fence, u64 value);

		void ExecuteCommandList(const GfxCommandList& cmdList);
		void ExecuteCommandLists(std::span<const GfxCommandList> cmdLists);

	private:
		ComPtr<DX12::CommandQueue> m_cmdQueue;
		u64                        m_timestampFrequency;
		D3D12_COMMAND_LIST_TYPE    m_type;
	};
}
