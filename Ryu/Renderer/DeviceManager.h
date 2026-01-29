#pragma once
#include "Core/Utils/Singleton.h"
#include "Renderer/GraphicsAPI.h"
#include <optional>
#include <nvrhi/nvrhi.h>
#include <dxgi1_6.h>
#include <d3dcommon.h>
#include <list>

namespace Ryu::Graphics
{
	struct DeviceCreateInfo
	{
		bool EnableDebugRuntime                  = false;
		bool EnableGPUValidationDX12             = false;
		bool EnableWarningsAsErrors              = false;
		bool IsHeadless                          = false;
		bool LogBufferLifetime                   = false;
		bool EnableHeapDirectlyIndexedDX12       = false;
		u32 BackBufferWidth                      = 0;
		u32 BackBufferHeight                     = 0;
		u32 RefreshRate                          = 0;
		u32 SwapChainBufferCount                 = 3;
		u32 SwapChainSampleCount                 = 1;
		u32 SwapChainSampleQuality               = 0;
		u32 MaxFramesInFlight                    = 2;
		nvrhi::Format SwapChainFormat            = nvrhi::Format::SRGBA8_UNORM;
		nvrhi::Format DepthBufferFormat          = nvrhi::Format::UNKNOWN;
		bool EnableNvrhiValidationLayer          = false;
		bool EnableComputeQueue                  = false;
		bool EnableCopyQueue                     = false;
		bool EnableVSync                         = false;
		i32 AdapterIndex                         = -1;  // Automatic by default
		nvrhi::IMessageCallback* MessageCallback = nullptr;

		DXGI_USAGE SwapChainUsage                = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		D3D_FEATURE_LEVEL FeatureLevel           = D3D_FEATURE_LEVEL_11_1;
	};

	struct AdapterInfo
	{
		using AdapterUUID = std::array<u8, 16>;
		using AdapterLUID = std::array<u8, 8>;

		std::string Name;
		u32 VendorID = 0;
		u32 DeviceID = 0;
		u64 DedicatedVideoMemory = 0;

		std::optional<AdapterUUID> UUID;
		std::optional<AdapterLUID> LUID;

		nvrhi::RefCountPtr<IDXGIAdapter> DXGIAdapter;
	};

	struct DefaultMessageCallback 
		: public Utils::Singleton<DefaultMessageCallback>
		, public nvrhi::IMessageCallback
	{
		void message(nvrhi::MessageSeverity severity, const char* messageText) override;
	};

	class IRenderPass;

	class DeviceManager
	{
	public:
		static DeviceManager* Create(API api);
		virtual ~DeviceManager() = default;

		const DeviceCreateInfo& GetDeviceCreateInfo() const { return m_createInfo; }
		u32 GetFrameIndex() const { return m_frameIndex; }
		nvrhi::ITexture* GetDepthBuffer() const { return m_depthBuffer; }
		nvrhi::IFramebuffer* GetCurrentFramebuffer(bool withDepth = true);
		nvrhi::IFramebuffer* GetFramebuffer(uint32_t index, bool withDepth = true);

		bool CreateDeviceAndSwapChain(const DeviceCreateInfo& info, HWND window);  // Create device dependent objects
		bool CreateInstance();                                                     // Create device independent objects
		void UpdateWindowSize();                                                   // Call to check internal window size and fire resize if neeeded

		void AddRenderPassToFront(IRenderPass* renderPass);
		void AddRenderPassToBack(IRenderPass* renderPass);
		void RemoveRenderPass(IRenderPass* renderPass);

		bool PreRender();
		void RenderPasses();
		bool PostRender();

	public:  // Public API
		virtual API GetAPI() const = 0;
		virtual bool EnumerateAdapters(std::vector<AdapterInfo>& outAdapters) = 0;
		virtual nvrhi::IDevice* GetDevice() const = 0;
		virtual std::string_view GetRendererName() const = 0;
		virtual nvrhi::ITexture* GetCurrentBackBuffer() = 0;
		virtual nvrhi::ITexture* GetBackBuffer(u32 index) = 0;
		virtual u32 GetCurrentBackBufferIndex() = 0;
		virtual u32 GetBackBufferCount() = 0;
		
		virtual void SetVSyncEnabled(bool enable) { m_requestedVSync = enable; }
		virtual void ReportLiveObjects() {}

		virtual void Shutdown();

	protected:
		DeviceManager() {}

		void BackBufferResizing();
		void BackBufferResized();
		void CreateDepthBuffer();
		void Render();

	protected:  // Protected API
		virtual bool CreateInstanceInternal() = 0;
		virtual bool CreateDevice() = 0;
		virtual bool CreateSwapChain() = 0;
		virtual void DestroyDeviceAndSwapChain() = 0;
		virtual void ResizeSwapChain() = 0;
		virtual bool BeginFrame() = 0;
		virtual bool Present() = 0;

	private:
		static DeviceManager* Create_DX12();

	protected:
		DeviceCreateInfo                      m_createInfo;
		bool                                  m_skipRenderOnFirstFrame = false;
		bool                                  m_requestedVSync         = false;
		bool                                  m_instanceCreated        = false;
		u32                                   m_frameIndex             = 0;
		HWND                                  m_hWnd                   = nullptr;
		std::vector<nvrhi::FramebufferHandle> m_swapChainFramebuffers;
		std::vector<nvrhi::FramebufferHandle> m_swapChainWithDepthFramebuffers;
		nvrhi::TextureHandle                  m_depthBuffer;
		std::list<IRenderPass*>               m_renderPasses;  // non-owning
	};

	class IRenderPass
	{
	public:
		IRenderPass(DeviceManager* deviceManager) : m_deviceManager(deviceManager) {}
		virtual ~IRenderPass() = default;

		virtual bool SupportsDepthBuffer() { return true; }
		virtual void Render([[maybe_unused]] nvrhi::IFramebuffer* framebuffer) {}
		virtual void BackBufferResizing() {}
		virtual void BackBufferResized(
			[[maybe_unused]] const u32 width, 
			[[maybe_unused]] const u32 height, 
			[[maybe_unused]] const u32 sampleCount) {}

		DeviceManager* GetDeviceManager() const { return m_deviceManager;                  }
		nvrhi::IDevice* GetDevice() const       { return m_deviceManager->GetDevice();     }
		u32 GetFrameIndex() const               { return m_deviceManager->GetFrameIndex(); }
	private:
		DeviceManager* m_deviceManager;
	};
};
