#pragma once
#include "Graphics/Core/Device.h"
#include "Graphics/Core/SwapChain.h"
#include "Graphics/Core/CommandAllocator.h"
#include "Graphics/Core/CommandQueue.h"
#include "Graphics/Core/CommandList.h"
#include "Graphics/Core/Fence.h"

namespace Ryu::Gfx
{
	class Renderer
	{
	public:
		Renderer() = delete;
		explicit Renderer(HWND window);
		~Renderer();

		inline NODISCARD const Device& GetDevice() const { return *m_device; }
		inline NODISCARD const SwapChain& GetSwapChain() const { return m_swapChain; }

		void Render(/*Scene, Camera*/);
		void OnResize(u32 width, u32 height);

	private:
		void PopulateCommandList();
		void WaitForGPU();
		void MoveToNextFrame();
		void CreateRootSignature();
		void CompileShaders();

	private:
		DevicePtr m_device;
		SwapChain m_swapChain;

		HANDLE                       m_fenceEvent{ nullptr };
		FrameArray<u64>              m_fenceValues{ 0 };
		Fence                        m_fence;
		CommandQueue                 m_cmdQueue;
		FrameArray<CommandAllocator> m_cmdAllocators;
		CommandList                  m_cmdList;
		DescriptorHeap               m_rtvHeap;
		ComPtr<DX12::RootSignature>  m_rootSignature;
	};
}
