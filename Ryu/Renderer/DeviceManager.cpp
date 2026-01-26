#include "Renderer/DeviceManager.h"
#include "Renderer/DeviceManagerDX12.h"
#include "Core/Logging/Logger.h"
#include <nvrhi/utils.h>

#if defined(_WIN32)
extern "C"
{
	// Declaring this symbol makes the OS run the app on the discrete GPU on NVIDIA Optimus laptops by default
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	// Same as above, for laptops with AMD GPUs
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace Ryu::Graphics
{
	DeviceManager* DeviceManager::Create_DX12() { return new DeviceManagerDX12(); }

	DeviceManager* DeviceManager::Create(API api)
	{
		switch (api)
		{
		case Ryu::Graphics::API::DirectX12: return Create_DX12();
		case Ryu::Graphics::API::DirectX11:
		case Ryu::Graphics::API::Vulkan:
			nvrhi::utils::NotSupported();
		default: return nullptr;
		}
	}
	
	nvrhi::IFramebuffer* DeviceManager::GetCurrentFramebuffer(bool withDepth)
	{
		return GetFramebuffer(GetCurrentBackBufferIndex(), withDepth);
	}

	nvrhi::IFramebuffer* DeviceManager::GetFramebuffer(uint32_t index, bool withDepth)
	{
		if (withDepth && index < m_swapChainWithDepthFramebuffers.size())
		{
			return m_swapChainWithDepthFramebuffers[index];
		}
		else if(index < m_swapChainFramebuffers.size())
		{
			
			return m_swapChainFramebuffers[index];
		}

		return nullptr;
	}

	bool DeviceManager::CreateDeviceAndSwapChain(const DeviceCreateInfo& info, HWND window)
	{
		m_hWnd                  = window;
		m_createInfo            = info;
		m_createInfo.IsHeadless = false;
		m_requestedVSync        = info.EnableVSync;

		if (!CreateInstance(m_createInfo))
		{
			return false;
		}

		if (!CreateDevice())
		{
			return false;
		}

		if (!CreateSwapChain())
		{
			return false;
		}

		// Force a resize event using invalid width and height
		m_createInfo.BackBufferWidth = 0;
		m_createInfo.BackBufferHeight = 0;

		UpdateWindowSize();

		return true;
	}
	
	bool DeviceManager::CreateInstance([[maybe_unused]] const InstanceCreateInfo& info)
	{
		if (m_instanceCreated)
		{
			return true;
		}

		m_instanceCreated = CreateInstanceInternal();
		return m_instanceCreated;
	}

	void DeviceManager::UpdateWindowSize()
	{
		RECT rc{};
		::GetClientRect(m_hWnd, &rc);

		i32 width = rc.right - rc.left;
		i32 height = rc.bottom - rc.top;

		if (width == 0 || height == 0)
		{
			RYU_LOG_DEBUG("Tried to resize but window ({}x{})is minimizes", width, height);
			return;
		}

		if (m_createInfo.BackBufferWidth != width
			|| m_createInfo.BackBufferHeight != height)
		{
			BackBufferResizing();

			m_createInfo.BackBufferWidth = width;
			m_createInfo.BackBufferHeight = height;

			ResizeSwapChain();
			BackBufferResized();
		}

		m_createInfo.EnableVSync = m_requestedVSync;
	}
	
	void DefaultMessageCallback::message(nvrhi::MessageSeverity severity, const char* messageText)
	{
		switch (severity)
		{
			using enum nvrhi::MessageSeverity;
		case Info    : RYU_LOG_INFO("{}", messageText);  break;
		case Warning : RYU_LOG_WARN("{}", messageText);  break;
		case Error   : RYU_LOG_ERROR("{}", messageText); break;
		case Fatal   : RYU_LOG_FATAL("{}", messageText); break;
		}
	}

	void DeviceManager::Shutdown()
	{
		m_swapChainFramebuffers.clear();
		m_swapChainWithDepthFramebuffers.clear();
		m_depthBuffer = nullptr;

		DestroyDeviceAndSwapChain();

		m_instanceCreated = false;
	}

	void DeviceManager::MAIN_RUN()
	{
		// Animate Render Present -> bool
		{
			if (m_frameIndex > 0 || !m_skipRenderOnFirstFrame)
			{
				if (BeginFrame())
				{
					// First time renderin this loop, m_frameIndex is 1 for m_skipRenderOnFirstFrame, 0 otherwise
					u32 frameindex = m_frameIndex;

					if (m_skipRenderOnFirstFrame)
					{
						frameindex--;
					}

					Render();
					bool presentSuccess = Present();

					if (!presentSuccess)
					{
						return /*false*/;
					}
				}
			}

			// NOTE: Run this in the main loop in Engine
			std::this_thread::sleep_for(std::chrono::milliseconds(0));

			GetDevice()->runGarbageCollection();
			++m_frameIndex;
		}

		[[maybe_unused]] bool waitSuccess = GetDevice()->waitForIdle();
		RYU_ASSERT(waitSuccess, "Wait for idle failed");
	}

	void DeviceManager::AddRenderPassToFront(IRenderPass* renderPass)
	{
		m_renderPasses.remove(renderPass);
		m_renderPasses.push_front(renderPass);

		renderPass->BackBufferResizing();
		renderPass->BackBufferResized(
			m_createInfo.BackBufferWidth,
			m_createInfo.BackBufferHeight,
			m_createInfo.SwapChainSampleCount);
	}

	void DeviceManager::AddRenderPassToBack(IRenderPass* renderPass)
	{
		m_renderPasses.remove(renderPass);
		m_renderPasses.push_back(renderPass);

		renderPass->BackBufferResizing();
		renderPass->BackBufferResized(
			m_createInfo.BackBufferWidth,
			m_createInfo.BackBufferHeight,
			m_createInfo.SwapChainSampleCount);
	}

	void DeviceManager::RemoveRenderPass(IRenderPass* renderPass)
	{
		m_renderPasses.remove(renderPass);
	}

	void DeviceManager::BackBufferResizing()
	{
		m_swapChainFramebuffers.clear();
		m_swapChainWithDepthFramebuffers.clear();

		//TODO: Handle render pass resizing
	}

	void DeviceManager::BackBufferResized()
	{
		CreateDepthBuffer();

		for (IRenderPass* pass : m_renderPasses)
		{
			pass->BackBufferResized(
				m_createInfo.BackBufferWidth,
				m_createInfo.BackBufferHeight,
				m_createInfo.SwapChainSampleCount);
		}

		u32 backBufferCount = GetBackBufferCount();
		m_swapChainFramebuffers.resize(backBufferCount);
		m_swapChainWithDepthFramebuffers.resize(backBufferCount);

		for (u32 i = 0; i < backBufferCount; i++)
		{
			nvrhi::FramebufferDesc desc = nvrhi::FramebufferDesc()
				.addColorAttachment(GetBackBuffer(i));

			m_swapChainFramebuffers[i] = GetDevice()->createFramebuffer(desc);

			if (m_depthBuffer)
			{
				desc.setDepthAttachment(m_depthBuffer);
				m_swapChainWithDepthFramebuffers[i] = GetDevice()->createFramebuffer(desc);
			}
			else
			{
				m_swapChainWithDepthFramebuffers[i] = m_swapChainFramebuffers[i];
			}
		}
	}

	void DeviceManager::CreateDepthBuffer()
	{
		m_depthBuffer = nullptr;

		if (m_createInfo.DepthBufferFormat == nvrhi::Format::UNKNOWN)
			return;

		nvrhi::TextureDesc textureDesc = nvrhi::TextureDesc()
			.setDebugName("Depth Buffer")
			.setWidth(m_createInfo.BackBufferWidth)
			.setHeight(m_createInfo.BackBufferHeight)
			.setFormat(m_createInfo.DepthBufferFormat)
			.setDimension(m_createInfo.SwapChainSampleCount > 1 
				? nvrhi::TextureDimension::Texture2DMS
				: nvrhi::TextureDimension::Texture2D)
			.setSampleCount(m_createInfo.SwapChainSampleCount)
			.setSampleQuality(m_createInfo.SwapChainSampleQuality)
			.setIsTypeless(true)
			.setIsRenderTarget(true)
			.enableAutomaticStateTracking(nvrhi::ResourceStates::DepthWrite);

		m_depthBuffer = GetDevice()->createTexture(textureDesc);
	}
	
	void DeviceManager::Render()
	{
		for (IRenderPass* pass : m_renderPasses)
		{
			nvrhi::IFramebuffer* buffer = GetCurrentFramebuffer(pass->SupportsDepthBuffer());
			pass->Render(buffer);
		}
	}
}
