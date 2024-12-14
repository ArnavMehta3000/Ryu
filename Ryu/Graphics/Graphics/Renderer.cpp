#include "Renderer.h"
#include "Graphics/Config.h"
#include "Graphics/Shared/Logging.h"
#include <libassert/assert.hpp>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include "Graphics/DX12/DX12Device.h"

namespace Ryu::Graphics
{
	VoidResult InitGraphics(Renderer* renderer, API api, HWND hWnd)
	{
		DEBUG_ASSERT(renderer, "Renderer is nullptr!");
		if (!renderer)
		{
			return MakeResultError{ "Renderer is nullptr!" };
		}

		GraphicsConfig& config = GraphicsConfig::Get();

		// If GPU based validation is enabled automatically enable the debug layer as well
		if (config.EnableGPUBasedValidation && !config.EnableDebugLayer)
		{
			config.EnableDebugLayer = true;
		}


		const DeviceCreateDesc deviceCreateDesc
		{
			.GraphicsAPI              = api,
			.EnableDebugLayer         = config.EnableDebugLayer,
			.EnableGPUBasedValidation = config.EnableGPUBasedValidation,
			.EnableVSync              = config.EnableVSync
		};
		
		const SwapChainDesc swapChainDesc
		{
			.Width         = 1280,
			.Height        = 720,
			.BufferCount   = FRAME_BUFFER_COUNT,
			.SampleCount   = 1,
			.SampleQuality = 0,
			.Flags         = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
			.Format        = Format::R8G8B8A8_UNORM,
			.SwapEffect    = SwapEffect::FlipSequential,
			.WindowHandle  = hWnd,
			.Windowed      = true
		};

		return renderer->Init(deviceCreateDesc, swapChainDesc);
	}

	void ShutdownGraphics(Renderer* renderer)
	{
		DEBUG_ASSERT(renderer, "Renderer is nullptr!");
		if (!renderer)
		{
			return;
		}

		renderer->Shutdown();
	}

	VoidResult Renderer::Init(const DeviceCreateDesc& deviceCreatedesc, const SwapChainDesc& swapChainDesc)
	{
		DXCall(::CoInitializeEx(NULL, COINIT_MULTITHREADED));

		auto deviceResult = IDevice::Create(deviceCreatedesc);
		if (!deviceResult)
		{
			return MakeResultError{ deviceResult.error() };
		}

		m_device = std::move(*deviceResult);
		InitializeResource(m_device.get());

		auto swapChainResult = m_device->CreateSwapChain(swapChainDesc);
		if (!swapChainResult)
		{
			return MakeResultError{ swapChainResult.error() };
		}

		m_swapchain = std::move(*swapChainResult);
		InitializeResource(m_swapchain.get());


		CommandListDesc cmdListDesc{ CommandListType::Graphics };
		m_commandLists.resize(swapChainDesc.BufferCount);
		
		for (uint32_t i = 0; i < swapChainDesc.BufferCount; i++)
		{
			auto createCmdListResult = m_device->CreateCommandList(cmdListDesc);
			if (!createCmdListResult)
			{
				return MakeResultError{ "Failed to create command list" };
			}
			
			m_commandLists[i] = std::move(*createCmdListResult);
		}
		
		return {};
	}
	
	void Renderer::Shutdown()
	{
		m_swapchain.reset();
		m_device.reset();
	
		::CoUninitialize();
	}
	
	void Renderer::InitializeResource(IGraphicsObject* obj)
	{
		if (obj)
		{
			DEBUG_ASSERT(obj->GetRenderer() == nullptr, "Graphics object already has a renderer!");
			
			if (obj->GetRenderer() != nullptr)
			{
				return;
			}

			obj->SetRenderer(this);
		}
	}
	
	void Renderer::BeginFrame()
	{
		u32 frameIndex = m_swapchain->GetCurrentFrameIndex();
		auto& cmdList = m_commandLists[frameIndex];
		cmdList->Begin();
	}
	
	void Renderer::EndFrame()
	{
		u32 frameIndex = m_swapchain->GetCurrentFrameIndex();
		auto& cmdList = m_commandLists[frameIndex];
		cmdList->End();
		m_device->ExecuteCommandList(cmdList.get());
		m_swapchain->Present();
	}
}
