#pragma once
#include <Graphics/Common.h>
#include <Graphics/DX12/DX12Device.h>
#include <array>

namespace Ryu::Graphics
{
	class DXGIFactory;
	class DX12CommandQueue;

	class DXGISwapChain : public ComPtr<IDXGISwapChain4>
	{
	public:
		static constexpr auto BACK_BUFFER_COUNT = 3;

		DXGISwapChain();
		~DXGISwapChain();

		void Create(HWND hWnd, const DX12Device& device, const DXGIFactory& factory, const DX12CommandQueue& cmdQueue, u32 width, u32 height, DXGI_FORMAT format);
		void Present();
		void Release();

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const;

	private:
		void ReleaseBuffers();
		void CreateDescriptorHeaps();
		void CreateBuffers();

	private:
		DX12Device::InterfaceType* m_device;
		std::array<ComPtr<ID3D12Resource>, BACK_BUFFER_COUNT> m_renderTargets;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		u32 m_currentBuffer;
		u32 m_width;
		u32 m_height;
		u32 m_heapIncrement;
	};
}
