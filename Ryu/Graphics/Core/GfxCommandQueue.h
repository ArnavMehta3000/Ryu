#pragma once
#include "Graphics/Core/GfxDeviceChild.h"
#include <span>

namespace Ryu::Gfx
{
	class Fence;
	class CommandList;

	class CommandQueue : public DeviceChild
	{
	public:
		CommandQueue(Device* parent, D3D12_COMMAND_LIST_TYPE type, std::string_view name);
		virtual ~CommandQueue() override = default;

		virtual inline void ReleaseObject() override { ComRelease(m_cmdQueue); }

		RYU_GFX_NATIVE(m_cmdQueue)

		void Signal(const Fence& fence, u64 value);

		void ExecuteCommandList(const CommandList& cmdList);
		void ExecuteCommandLists(std::span<const CommandList> cmdLists);

	private:
		ComPtr<DX12::CommandQueue> m_cmdQueue;
		u64                        m_timestampFrequency;
		D3D12_COMMAND_LIST_TYPE    m_type;
	};
}
