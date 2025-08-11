#pragma once
#include "Graphics/Core/Device.h"
#include "Graphics/Core/SwapChain.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Core/PipelineState.h"
#include <optional>

#if defined(RYU_WITH_EDITOR)
#include "Graphics/ImGui/ImGuiRenderer.h"
#endif

namespace Ryu::Gfx
{
	class Renderer
	{
	public:
		Renderer() = delete;
		explicit Renderer(HWND window);
		~Renderer() = default;

		void Initialize();
		void Shutdown();

		inline NODISCARD Device& GetDevice() { return *m_device; }
		inline NODISCARD SwapChain& GetSwapChain() { return m_swapChain; }
		
#if defined(RYU_WITH_EDITOR)
		inline void SetImGuiCallback(std::function<void(Renderer*)> callback) { m_imguiCallback = callback; }
#endif
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
		HWND m_window;
		DevicePtr m_device;
		SwapChain m_swapChain;

		DescriptorHeap m_rtvDescHeap;
		DescriptorHeap m_dsvDescHeap;
		DescriptorHeap m_srvDescHeap;
		DescriptorHeap m_uavDescHeap;

		PipelineState                m_pso;
		ComPtr<DX12::RootSignature>  m_rootSignature;
		ComPtr<DX12::Resource>       m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW     m_vertexBufferView;

#if defined(RYU_WITH_EDITOR)
		std::optional<std::function<void(Renderer*)>> m_imguiCallback;
		std::unique_ptr<ImGuiRenderer> m_imguiRenderer{ nullptr };
#endif

	};
}
