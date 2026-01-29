#include "Graphics/RendererBase.h"
#include "Graphics/Renderer.h"
#include "Core/Logging/Logger.h"
#include <nvrhi/utils.h>

namespace Ryu::Gfx
{
	RendererBase* RendererBase::Create(nvrhi::GraphicsAPI api)
	{
		switch (api)
		{
		case nvrhi::GraphicsAPI::D3D12: return new RendererDX12();
		case nvrhi::GraphicsAPI::D3D11:
		case nvrhi::GraphicsAPI::VULKAN:
		default:
			nvrhi::utils::NotSupported();
			return nullptr;
		}
	}

	bool RendererBase::Init(const RendererInitInfo& info, HWND targetWindow)
	{
		m_hWnd = targetWindow;
		m_initInfo = info;

		if (!CreateInstance())
		{
			RYU_LOG_ERROR("Failed to create renderer instance!");
			return false;
		}

		if (!CreateDevice())
		{
			RYU_LOG_ERROR("Failed to create renderer device!");
			return false;
		}

		if (!CreateSwapChain())
		{
			RYU_LOG_ERROR("Failed to create swap chain!");
			return false;
		}

		// Force a resize event using invalid width and height
		m_initInfo.BackBufferSize = { 0, 0 };
		
		Resize();

		return false;
	}

	void RendererBase::Shutdown()
	{
	}
	
	bool RendererBase::CreateInstance()
	{
		if (m_instanceCreated)
		{
			return true;
		}
		m_instanceCreated = CreateDeviceIndependentResources();
		return m_instanceCreated;
	}
	
	void RendererBase::message(nvrhi::MessageSeverity severity, const char* messageText)
	{
		switch (severity)
		{
			using enum nvrhi::MessageSeverity;
		case Info           : RYU_LOG_INFO("{}", messageText);  break;
		case Warning        : RYU_LOG_WARN("{}", messageText);  break;
		case Error          : RYU_LOG_ERROR("{}", messageText); break;
		case Fatal          : RYU_LOG_FATAL("{}", messageText); break;
		}
	}
}

