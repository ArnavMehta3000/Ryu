#pragma once
#include <Graphics/Common.h>
#include <Graphics/DX12/DX12Resource.h>
#include <Graphics/DX12/DX12DescriptorHeap.h>
#include <array>

namespace Ryu::Graphics
{
	class DX12Device;
	class DXGIFactory;
	class DX12GraphicsCommandList;
	class DX12CommandQueue;

	class DXGISwapChain : public ComPtr<IDXGISwapChain4>
	{
	public:
		static constexpr auto BACK_BUFFER_COUNT = 2;

		DXGISwapChain(InterfaceType* ptr);
		DXGISwapChain();
		~DXGISwapChain() = default;

		static NODISCARD CreateResult<DXGISwapChain> Create(
			const HWND hWnd, const DX12Device& device, const DXGIFactory& factory, 
			const DX12CommandQueue& cmdQueue, u32 width, u32 height, 
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool vsync = true);

		void Release();
		void Resize(const DX12Device& device, const DX12GraphicsCommandList& cmdList, u32 width, u32 height);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
		const DX12Resource& GetCurrentBackBuffer() const;
		void Present();

	private:
		void CreateDescriptorHeaps(const DX12Device& device);

	private:
		std::array<DX12Resource, BACK_BUFFER_COUNT> m_backBuffers;
		DX12Resource                                m_depthStencilBuffer;
		DX12DescriptorHeap                          m_rtvHeap;
		DX12DescriptorHeap                          m_dsvHeap;
		DescriptorSizes                             m_descriptorSizes;
		DXGI_FORMAT                                 m_format;
		u32                                         m_currentBackBufferIndex;
	};
}
