#include "Engine/Setup/Setup.h"
#include "Engine/Setup/EngineMain.h"
#include "Testbench/TestbenchApp.h"

#include "Renderer/DeviceManager.h"
#include <nvrhi/utils.h>

using namespace Ryu;
using namespace Ryu::Graphics;

class ClearPass : public IRenderPass
{
public:
	ClearPass(DeviceManager* deviceManager)
		: IRenderPass(deviceManager) 
	{
		m_cmdList = GetDevice()->createCommandList();
	}

	void Render(nvrhi::IFramebuffer* framebuffer) override
	{
		m_cmdList->open();
		nvrhi::utils::ClearColorAttachment(m_cmdList, framebuffer, 0, nvrhi::Color(0.15f));
		m_cmdList->close();

		GetDevice()->executeCommandList(m_cmdList);
	}

private:
	nvrhi::CommandListHandle m_cmdList;
};

bool TestNVRHI()
{
	// Create window
	Window::Window::Config windowConfig
	{
		.Title               = "Testbench",
		.CanBorderlessResize = true,
		.CanBorderlessDrag   = true,
		.Type                = Window::WindowType::Borderless
	};

	auto window = std::make_unique<Window::Window>(windowConfig);
	App::App::InitWindow(*window);

	std::unique_ptr<DeviceManager> devMan(DeviceManager::Create(API::DirectX12));

	DeviceCreateInfo info{};
	info.DepthBufferFormat = nvrhi::Format::D32;
	info.EnableWarningsAsErrors = false;
	info.EnableNvrhiValidationLayer
		= info.EnableDebugRuntime
		= info.EnableGPUValidationDX12 = true;
	info.LogBufferLifetime = true;


	Event::ScopedListener<Window::ResizeEvent> resizeListener(window->GetDispatcher(),
		[&devMan](const Window::ResizeEvent&)
		{
			devMan->UpdateWindowSize();
		});

	Event::ScopedListener<Window::KeyEvent> keyListener(window->GetDispatcher(),
		[&window](const Window::KeyEvent&)
		{
			window->RequestClose();
		});

	if (devMan->CreateDeviceAndSwapChain(info, *window))
	{
		ClearPass pass(devMan.get());

		devMan->AddRenderPassToBack(&pass);

		while (window->IsOpen() && window->GetHandle())
		{
			window->Update();
			devMan->MAIN_RUN();
			window->ProcessEventQueue();
		}
	}

	devMan->Shutdown();

	return true;
}

#if !defined(RYU_WITH_EDITOR)
RYU_MAIN()
{
	RYU_DBG_TRACK_MEM();

	if (TestNVRHI())
	{
		return 0;
	}

	try
	{
		if (!Engine::Setup())
		{
			Engine::Shutdown();
			return -1;
		}

		Window::Window::Config windowConfig
		{
			.Title               = "Testbench",
			.CanBorderlessResize = true,
			.CanBorderlessDrag   = true,
			.Type                = Window::WindowType::Borderless
		};

		// Create window
		auto window = std::make_shared<Window::Window>(windowConfig);
		App::App::InitWindow(*window);

		// Run app
		Ryu::Engine::Engine::Get().RunApp(std::make_shared<TestbenchApp>(window));
		return 0;
	}
	catch (const AssertException& e)
	{
		RYU_LOG_FATAL("{}", e.what());

		Engine::Shutdown();
	}
}
#endif
