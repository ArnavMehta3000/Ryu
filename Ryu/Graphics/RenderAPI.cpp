#include "RenderAPI.h"
#include <Core/Utils/StringConv.h>
#include <Graphics/DXGI/DXGIFactory.h>
#include <Graphics/DXGI/DXGIAdapter.h>
#include <Graphics/Debug/DX12Debug.h>
#include <DirectXColors.h>

namespace Ryu::Graphics
{
	RenderAPI::RenderAPI()
		: m_hWnd(nullptr)
		, m_currentFence(0)
		, m_width(0)
		, m_height(0)
	{
	}
	
	bool RenderAPI::Initialize(HWND hWnd, u32 width, u32 height)
	{
		RYU_GFX_INFO("Initializing RenderAPI");

		m_hWnd = hWnd;
		m_width = width;
		m_height = height;

#if defined(RYU_BUILD_DEBUG)
		if (auto result = DX12Debug::Create(); result)
		{
			result.value()->EnableDebugLayer();
		}
#endif

		// Create factory
		DXGIFactory factory;
		DXGIAdapter adapter;
		if (auto result = DXGIFactory::Create(); result)
		{
			factory = result.value();
			adapter = factory.GetAdapter();

			DXGI_ADAPTER_DESC3 desc{};
			adapter->GetDesc3(&desc);
			std::string name = ToNarrowStr(desc.Description);
			RYU_GFX_DEBUG("Selected adapter: {} | Vendor Id: {}", name, desc.VendorId);
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DXGI factory. Error: {}", HRToString(result.error()));
		}

		// Create device
		if (auto result = DX12Device::Create(adapter); result)
		{
			m_device = std::move(result.value());
			RYU_GFX_ASSERTHR(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())),
				"Failed to create DX12Fence");
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DX12Device. Error: {}", HRToString(result.error()));
		}

		// Create command queue
		if (auto result = DX12CommandQueue::Create(m_device); result)
		{
			m_commandQueue = std::move(result.value());
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DX12CommandQueue. Error: {}", HRToString(result.error()));
		}

		// Create command list
		if (auto result = DX12GraphicsCommandList::Create(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT); result)
		{
			m_commandList = std::move(result.value());
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DX12GraphicsCommandList. Error: {}", HRToString(result.error()));
		}

		// Create swapchain
		if (auto result = DXGISwapChain::Create(hWnd, m_device, factory, m_commandQueue, width, height, DXGI_FORMAT_R8G8B8A8_UNORM); result)
		{
			m_swapChain = std::move(result.value());
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DXGISwapChain. Error: {}", HRToString(result.error()));
		}

		// Perform initial resize
		OnResize(width, height);

		RYU_GFX_TRACE("Finished initializing RenderAPI");
		return true;
	}

	void RenderAPI::Shutdown()
	{
		m_fence.Reset();
		m_commandList.Release();
		m_commandQueue.Release();
		m_swapChain.Release();
		m_device.Release();
	}

	void RenderAPI::Present()
	{
		m_commandList.ResetCommandList();

		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);

		// Start state transition
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain.GetCurrentBackBuffer().Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			m_commandList->ResourceBarrier(1, &barrier);
		}

		// Clear back and depth buffers
		const auto currentRTV = m_swapChain.GetCurrentBackBufferView();
		const auto currentDSV = m_swapChain.GetDepthStencilView();
		m_commandList->ClearRenderTargetView(currentRTV, DirectX::Colors::BlanchedAlmond, 0, nullptr);
		m_commandList->ClearDepthStencilView(m_swapChain.GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffers to render to
		m_commandList->OMSetRenderTargets(1, &currentRTV, TRUE, &currentDSV);

		// Transition back buffer to present
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain.GetCurrentBackBuffer().Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			m_commandList->ResourceBarrier(1, &barrier);
		}

		RYU_GFX_ASSERTHR(m_commandList->Close(), "Failed to close DX12GraphicsCommandList");

		ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		m_swapChain.Present();

		FlushCommandQueue();
	}

	void RenderAPI::OnResize(u32 width, u32 height)
	{
		RYU_GFX_ASSERT(m_device && m_swapChain && m_commandList.GetAllocator(), "RenderAPI not initialized");

		m_width = width;
		m_height = height;
		
		FlushCommandQueue();
		m_commandList.ResetCommandList();
		m_swapChain.Resize(m_device, m_commandList, width, height);
		
		RYU_GFX_ASSERTHR(m_commandList->Close(), "Failed to close DX12GraphicsCommandList");

		ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		FlushCommandQueue();

		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (f32)width, (f32)height);
		m_scissorRect = CD3DX12_RECT(0, 0, width, height);
	}

	void RenderAPI::FlushCommandQueue()
	{
		m_currentFence++;
		RYU_GFX_ASSERTHR(m_commandQueue->Signal(m_fence.Get(), m_currentFence), "Failed to signal fence");

		if (m_fence->GetCompletedValue() < m_currentFence)
		{
			if (HANDLE eventHandle = CreateEventEx(nullptr, NULL, false, EVENT_ALL_ACCESS); eventHandle != NULL)
			{
				RYU_GFX_ASSERTHR(m_fence->SetEventOnCompletion(m_currentFence, eventHandle), "Failed to set fence event");

				WaitForSingleObject(eventHandle, INFINITE);
				CloseHandle(eventHandle);
			}
		}
	}
}
