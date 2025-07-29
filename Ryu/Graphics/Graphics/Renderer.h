#pragma once
#include "Graphics/Core/Device.h"
#include "Graphics/Core/SwapChain.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/PipelineState.h"
#include "Graphics/Core/Fence.h"
#include <optional>

namespace Ryu::Gfx
{
	class Renderer
	{
	public:
		Renderer() = delete;
		explicit Renderer(HWND window);
		~Renderer();

		inline NODISCARD Device& GetDevice() { return *m_device; }
		inline NODISCARD SwapChain& GetSwapChain() { return m_swapChain; }
		
		inline void SetImGuiCallback(std::function<void(Renderer*)> callback) { m_imguiCallback = callback; }
		
		void ProcessDeferredReleases(u32 frameIndex);
		void DeferredRelease(IUnknown* resource);

		void Render(/*Scene, Camera*/);
		void OnResize(u32 width, u32 height);

	private:
		void PopulateCommandList();
		void CreateRootSignature();
		void CompileShaders();
		void CreateVB();

	private:
		DevicePtr m_device;
		SwapChain m_swapChain;

		DescriptorHeap                     m_rtvDescHeap;
		DescriptorHeap                     m_dsvDescHeap;
		DescriptorHeap                     m_srvDescHeap;
		DescriptorHeap                     m_uavDescHeap;

		PipelineState                m_pso;
		ComPtr<DX12::RootSignature>  m_rootSignature;
		ComPtr<DX12::Resource>       m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW     m_vertexBufferView;

		std::optional<std::function<void(Renderer*)>> m_imguiCallback;
	};
}
