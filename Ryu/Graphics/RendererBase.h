#pragma once
#include "Graphics/IRenderPass.h"
#include <d3dcommon.h>
#include <dxgi1_6.h>
#include <optional>

namespace Ryu::Gfx
{
    struct RendererInitInfo
    {
        bool EnableDebugRuntime         : 1 = false;
        bool EnableGPUValidation        : 1 = false;  // Only affects DX12
        bool LogBufferLifetime          : 1 = true;
        bool EnableHeapDirectlyIndexed  : 1 = false;  // Only affects DX12
        bool EnableWarningsAsErrors     : 1 = false;  // Will assert/crash when reporting live objects (DX12)
        bool EnableNVRHIValidationLayer : 1 = false;
        bool EnableComputeQueue         : 1 = false;
        bool EnableCopyQueue            : 1 = false;
        bool EnableVSync                : 1 = false;

        nvrhi::Format SwapChainFormat   = nvrhi::Format::SRGBA8_UNORM;
        nvrhi::Format DepthBufferFormat = nvrhi::Format::UNKNOWN;  // If unkown, then depth buffer is not created
        
        std::pair<u32, u32> BackBufferSize = { 0, 0 };
        u32 RefreshRate                    = 0;
        u32 SwapChainBufferCount           = 3;
        u32 SwapChainSampleCount           = 1;
        u32 SwapChainSampleQuality         = 0;
    
        DXGI_USAGE SwapChainUsage      = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_1;
    };

    struct AdapterInfo
    {
        using AdapterUUID = std::array<u8, 16>;
        using AdapterLUID = std::array<u8, 8>;

        std::string Name;
        u32 VendorID             = 0;
        u32 DeviceID             = 0;
        u64 DedicatedVideoMemory = 0;

        std::optional<AdapterUUID> UUID;
        std::optional<AdapterLUID> LUID;

        nvrhi::RefCountPtr<IDXGIAdapter> DXGIAdapter;
    };

    class IRendererInterface
    {
    public:
        virtual bool Init(const RendererInitInfo& info, HWND targetWindow) = 0;
        virtual void Shutdown() = 0;
        virtual bool EnumerateAdapters(std::vector<AdapterInfo>& outAdapters) = 0;
        virtual std::string_view GetRendererName() const = 0;
        virtual nvrhi::IDevice* GetDevice() const = 0;
        virtual nvrhi::ITexture* GetCurrentBackBuffer() const = 0;
        virtual nvrhi::ITexture* GetBackBuffer(u32 index) const = 0;
        virtual u32 GetCurrentBackBufferIndex() const = 0;
        virtual u32 GetBackBufferCount() const = 0;       
        virtual void ReportLiveObjects() = 0;
    
    protected:
        virtual bool CreateDeviceIndependentResources() = 0;
        virtual bool CreateDevice() = 0;
        virtual bool CreateSwapChain() = 0;
        virtual void DestroyDeviceAndSwapChain() = 0;
        virtual void ResizeSwapChain() = 0;
        virtual bool BeginFrame() = 0;
        virtual bool Present() = 0;
    };

    class RendererBase 
        : public IRendererInterface
        , public nvrhi::IMessageCallback
    {
    public:
        static RendererBase* Create(nvrhi::GraphicsAPI api = nvrhi::GraphicsAPI::D3D12);
        virtual ~RendererBase() = default;

        bool Init(const RendererInitInfo& info, HWND targetWindow) override final;
        virtual void Shutdown() override;

        void Resize();

        [[nodiscard]] inline const RendererInitInfo& GetInitInfo() const { return m_initInfo; }
        [[nodiscard]] inline u32 GetFrameIndex() const { return m_frameIndex; }
        [[nodiscard]] inline nvrhi::ITexture* GetDepthBuffer() const { return m_depthBuffer; }

        [[nodiscard]] nvrhi::IFramebuffer* GetCurrentFrameBuffer(bool withDepth = true);
		[[nodiscard]] nvrhi::IFramebuffer* GetFrameBuffer(u32 index, bool withDepth = true);

    protected:
        RendererBase() {}

        bool CreateInstance();
        void BackBufferResizeBegin();
		void BackBufferResizeEnd();
        void CreateDepthBuffer();
        
        // IMessageCallback
        void message(nvrhi::MessageSeverity severity, const char* messageText) override;

    protected:
        RendererInitInfo                      m_initInfo;
        u32                                   m_frameIndex       = 0;
        HWND                                  m_hWnd             = nullptr;
        bool                                  m_instanceCreated  = false;
        std::vector<nvrhi::FramebufferHandle> m_swapChainFrameBuffers;
        std::vector<nvrhi::FramebufferHandle> m_swapChainWithDepthFrameBuffers;
        nvrhi::TextureHandle                  m_depthBuffer;
        std::list<IRenderPass*>               m_renderPasses;  // non-owning
    };
}
