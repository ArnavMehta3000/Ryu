#pragma once
#include "Graphics/Core/DX12.h"
#include <span>

namespace Ryu::Gfx
{
	class GfxDevice;
	class GfxFence;
	class GfxCommandList;

	class GfxCommandQueue
	{
	public:
		GfxCommandQueue() = default;
		~GfxCommandQueue() = default;

		void Create(GfxDevice* device, CommandListType type, std::string_view name = "");

		void Signal(GfxFence& fence, u64 fenceValue);
		void Wait(GfxFence& fence, u64 fenceValue);

		void ExecuteCommandLists(std::span<GfxCommandList*> cmdLists);


		RYU_GFX_NATIVE(m_cmdQueue)
		RYU_GETTER_FUNC(u64, GetTimestampFrequency, m_timestampFrequency)
		RYU_GETTER_FUNC(CommandListType, GetType, m_type)

	private:
		ComPtr<DX12::CommandQueue> m_cmdQueue;
		u64 m_timestampFrequency = 0;
		CommandListType m_type;
	};

}
