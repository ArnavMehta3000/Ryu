#pragma once
#include "Graphics/Core/DX12.h"

namespace Ryu::Gfx
{
	class GfxDevice;
	class GfxCommandQueue;
	class GfxFence;

	class GfxCommandList
	{
	public:
		explicit GfxCommandList(GfxDevice* device, CommandListType type = CommandListType::Direct, std::string_view name = "");
		~GfxCommandList() = default;

		RYU_GFX_NATIVE(m_cmdList)

		void ResetAllocator();
		void Begin();
		void End();
		void Wait(GfxFence& fence, u64 value);
		void Signal(GfxFence& fence, u64 value);
		void WaitAll();
		void Submit();
		void SignalAll();

	private:
		GfxDevice* m_device = nullptr;
		CommandListType m_type;
		GfxCommandQueue& m_cmdQueue;
		ComPtr<DX12::GraphicsCommandList> m_cmdList;
		ComPtr<DX12::CommandAllocator> m_cmdAllocator;
	};
}
