#pragma once
#include "Graphics/Core/GfxFence.h"
#include "Graphics/Core/GfxSwapChain.h"
#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxCommandQueue.h"
#include "Graphics/Core/GfxDescriptorAllocator.h"
#include <queue>

namespace Ryu::Gfx
{
	class GfxDevice
	{
	public:
		explicit GfxDevice(HWND window);
		~GfxDevice();

		[[nodiscard]] inline HWND GetHWND() const noexcept { return m_hWnd; }
		[[nodiscard]] inline auto GetDevice() const noexcept { return m_device.Get(); }
		[[nodiscard]] inline auto GetFactory() const noexcept { return m_dxgiFactory.Get(); }
		[[nodiscard]] inline auto& GetCapabilities() const noexcept { return m_capabilities; }
		[[nodiscard]] inline GfxFence& GetGraphicsFence() noexcept { return m_graphicsFence; }
		//[[nodiscard]] inline GfxFence& GetComputeFence() noexcept { return m_computeFence; }
		//[[nodiscard]] inline GfxFence& GetCopyFence() noexcept { return m_copyFence; }
		[[nodiscard]] inline u64 GetGraphicsFenceValue() const noexcept { return m_graphicsFenceValue; }
		//[[nodiscard]] inline u64 GetComputeFenceValue() const noexcept { return m_computeFenceValue; }
		//[[nodiscard]] inline u64 GetCopyFenceValue() const noexcept { return m_copyFenceValue; }

		[[nodiscard]] GfxCommandQueue& GetCommandQueue(CommandListType type);
		[[nodiscard]] GfxCommandQueue& GetGraphicsCommandQueue() { return GetCommandQueue(CommandListType::Direct); }
		//[[nodiscard]] GfxCommandQueue& GetComputeCommandQueue() { return GetCommandQueue(CommandListType::Compute); }
		//[[nodiscard]] GfxCommandQueue& GetCopyCommandQueue() { return GetCommandQueue(CommandListType::Copy); }

		inline void SetGraphicsFenceValue(u64 value) { m_graphicsFenceValue = value; }
		//inline void SetComputeFenceValue(u64 value) { m_computeFenceValue = value; }
		//inline void SetCopyFenceValue(u64 value) { m_copyFenceValue = value; }

		void WaitForGPU();
		void BeginFrame();
		void EndFrame();

		template <Releasable T>
		void AddToReleaseQueue(T* ptr) 
		{
			m_releaseQueue.emplace(new ReleasableResource(ptr), m_releaseFenceValue);
		}

		GfxDescriptor AllocateDescriptorCPU(DescriptorHeapType type);
		void FreeDescriptorCPU(GfxDescriptor descriptor, DescriptorHeapType type);
		std::unique_ptr<GfxTexture> CreateBackBufferTexture(const GfxTexture::Desc& desc, DX12::Resource* backbuffer);
		//GfxDescriptor CreateTextureRTV(GfxTexture* texture /*, GfxTextureDescriptorDesc const* = nullptr*/);
	
	private:
		//GfxDescriptor CreateTextureView(GfxTexture* texture, D3D12_RESOUR);
		void ProcessReleaseQueue();

	private:
		HWND m_hWnd;
		u32  m_width, m_height;
		u32  m_frameIndex = 0;

		ComPtr<DX12::Device>  m_device;
		ComPtr<DXGI::Factory> m_dxgiFactory;
		CD3DX12FeatureSupport m_capabilities;

		std::unique_ptr<GfxSwapChain> m_swapChain;

		GfxCommandQueue m_graphicsQueue;
		//GfxCommandQueue m_computeQueue;
		//GfxCommandQueue m_copyQueue;

		GfxFence        m_frameFence;
		u64             m_frameFenceValue = 1;
		FrameArray<u64> m_frameFenceValues{};

		FrameArray<std::unique_ptr<GfxCommandList>> m_graphicsCmdLists;
		GfxFence m_graphicsFence;
		u64 m_graphicsFenceValue = 0;

		//GfxFence m_computeFence;
		//u64 m_computeFenceValue = 0;
		
		//GfxFence m_copyFence;
		//u64 m_copyFenceValue = 0;

		GfxFence m_waitFence;
		u64 m_waitFenceValue = 1;

		GfxFence m_releaseFence;
		u64 m_releaseFenceValue = 1;

		std::array<
			std::unique_ptr<GfxDescriptorAllocator>, (u64)DescriptorHeapType::_COUNT> m_descriptorAllocatorsCPU;
	
		struct ReleasableItem
		{
			std::unique_ptr<ReleasableObject> Obj;
			u64 FenceValue;

			ReleasableItem(ReleasableObject* obj, u64 fenceValue) : Obj(obj), FenceValue(fenceValue) {}
		};
		std::queue<ReleasableItem>  m_releaseQueue;
		bool m_isFirstFrame;
	};
}
