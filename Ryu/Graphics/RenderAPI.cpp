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
		DX12Debug dbg;
		dbg->SetEnableAutoName(TRUE);
#endif

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();

		{
			DXGI_ADAPTER_DESC3 desc{};
			adapter->GetDesc3(&desc);
			std::string name = ToNarrowStr(desc.Description);
			RYU_GFX_DEBUG("Selected adapter: {} | Vendor Id: {}", name, desc.VendorId);
		}

		m_device.Create(adapter);
		m_commandQueue.Create(m_device, D3D12_COMMAND_QUEUE_PRIORITY_HIGH);
		m_commandList.Create(m_device);
		m_swapChain.Create(hWnd, m_device, factory, m_commandQueue, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

		RYU_GFX_TRACE("Finished initializing RenderAPI");
		return true;
	}

	void RenderAPI::Shutdown()
	{
		m_swapChain.Release();
		m_commandList.Release();
		m_commandQueue.Release();
		m_device.Release();
	}
}
