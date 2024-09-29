#include "RenderAPI.h"
#include <Core/Utils/StringConv.h>
#include <Graphics/DXGI/DXGIFactory.h>
#include <Graphics/DXGI/DXGIAdapter.h>
#include <Graphics/Debug/DX12Debug.h>

namespace Ryu::Graphics
{
	RenderAPI::RenderAPI()
	{
	}
	
	RenderAPI::~RenderAPI()
	{
	}

	bool RenderAPI::Initialize(HWND hWnd, u32 width, u32 height)
	{
		RYU_GFX_INFO("Initializing RenderAPI");

#if defined(RYU_BUILD_DEBUG)
		if (auto result = DX12Debug::Create(); result)
		{
			DX12Debug dbg(result.value());
			dbg->EnableDebugLayer();
		}
#endif

		// Create factory
		DXGIFactory factory;
		if (auto result = DXGIFactory::Create(); result)
		{
			factory.Attach(result.value());
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DXGI factory. Error: {}", HRToString(result.error()));
		}

		// Get adapter
		DXGIAdapter adapter;
		factory.GetAdapter(adapter);

		{  // Print debugging info
			DXGI_ADAPTER_DESC3 desc{};
			adapter->GetDesc3(&desc);
			std::string name = ToNarrowStr(desc.Description);
			RYU_GFX_DEBUG("Selected adapter: {} | Vendor Id: {}", name, desc.VendorId);
		}

		// Create device
		if (auto result = DX12Device::Create(adapter); result)
		{
			m_device = DX12Device(result.value());
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DX12Device. Error: {}", HRToString(result.error()));
		}

		// Create command queue
		if (auto result = DX12CommandQueue::Create(m_device, D3D12_COMMAND_QUEUE_PRIORITY_HIGH); result)
		{
			m_commandQueue = DX12CommandQueue(result.value());
			m_commandQueue.Init(m_device);
		}
		else
		{
			RYU_GFX_FATAL("Failed to create DX12CommandQueue. Error: {}", HRToString(result.error()));
		}

		m_commandList.Create(m_device);
		//m_swapChain.Create(hWnd, m_device, factory, m_commandQueue, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

		RYU_GFX_TRACE("Finished initializing RenderAPI");
		return true;
	}

	void RenderAPI::Shutdown()
	{
		//m_swapChain.Release();
		m_commandList.Release();
		m_commandQueue.Release();
		m_device.Release();
	}

	void RenderAPI::Render()
	{
		//ID3D12GraphicsCommandList* gfxCmdList = m_commandList.GetGFXCmdList();

		//// Create transition barriers
		//{
		//	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		//		m_swapChain.GetCurrentRenderTarget(),
		//		D3D12_RESOURCE_STATE_PRESENT,
		//		D3D12_RESOURCE_STATE_RENDER_TARGET);

		//	gfxCmdList->ResourceBarrier(1, &barrier);
		//}

		//// Clear back current back buffer
		//const std::array<f32, 4> clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain.GetCurrentRTVHandle();

		//gfxCmdList->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);
		//gfxCmdList->Close();

		//const std::array cmdLists = { m_commandList.Get() };
		//m_commandQueue->ExecuteCommandLists(1, cmdLists.data());

		//m_commandList.ResetCommandList();
	}
}
