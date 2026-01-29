#include "Graphics/Renderer.h"

#include "Core/Profiling/Profiling.h"
#include "Core/Utils/StringConv.h"
#include "Core/Utils/Timing/FrameTimer.h"
#include "Game/World/World.h"
#include "Graphics/CommonStates.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/IRendererHook.h"
#include "Graphics/RenderFrameBuilder.h"
#include <dxgidebug.h>
#include <nvrhi/validation.h>

#define HR_RETURN(hr) do { if (FAILED(hr)) { return false; } } while (false)

namespace Ryu::Gfx
{
    Renderer::Renderer(HWND window, IRendererHook* hook)
        : m_device(std::make_unique<Device>(window))
        , m_gpuFactory(m_device.get())
        , m_assets(&m_gpuFactory)
        , m_shaderLibrary("./Shaders/Compiled"sv)
        , m_worldRenderer(m_device.get(), &m_shaderLibrary, &m_assets)
        , m_hook(hook)
    {
        RYU_PROFILE_SCOPE();

        m_device->Initialize();

        const auto [w, h] = m_device->GetClientSize();

#if defined(RYU_WITH_EDITOR)
        if (m_hook)
        {
            m_hook->OnImGuiSetup(m_device.get());
        }
#endif
    }

    Renderer::~Renderer()
    {
#if defined(RYU_WITH_EDITOR)
        if (m_hook)
        {
            m_hook->OnImGuiShutdown();
        }
#endif
    }

    void Renderer::RenderWorld(Game::World& world, const Utils::FrameTimer& frameTimer)
    {
        RYU_PROFILE_SCOPE();

        RenderFrameBuilder builder(&m_assets, m_device.get());

        const Gfx::RenderFrame frameData = builder.ExtractRenderData(world, frameTimer);
        m_worldRenderer.RenderFrame(frameData, &m_gpuFactory, m_hook);
    }

    void Renderer::OnResize(u32 w, u32 h)
    {
        RYU_PROFILE_SCOPE();

        m_worldRenderer.OnResize(w, h);  // This will call resize on the device;
    }
    
    
    
    RendererDX12::RendererDX12() = default;

    std::string RendererDX12::GetAdapterName(const DXGI_ADAPTER_DESC& desc)
    {
        return Ryu::Utils::ToNarrowStr(desc.Description);
    }

    bool RendererDX12::EnumerateAdapters(std::vector<AdapterInfo>& outAdapters)
    {
        if (!m_dxgiFactory)
		{
			return false;
		}

		outAdapters.clear();

		while (true)
		{
			nvrhi::RefCountPtr<IDXGIAdapter> adapter;

			if (FAILED(m_dxgiFactory->EnumAdapters(u32(outAdapters.size()), &adapter)))
			{
				return false;
			}

			DXGI_ADAPTER_DESC desc;
			if (FAILED(adapter->GetDesc(&desc)))
			{
				return false;
			}

			AdapterInfo adapterInfo
			{
				.Name                 = GetAdapterName(desc),
				.VendorID             = desc.VendorId,
				.DeviceID             = desc.DeviceId,
				.DedicatedVideoMemory = desc.DedicatedVideoMemory,
				.DXGIAdapter          = adapter
			};

			AdapterInfo::AdapterLUID luid;			
			static_assert(luid.size() == sizeof(desc.AdapterLuid));
			
			std::memcpy(luid.data(), &desc.AdapterLuid, luid.size());
			adapterInfo.LUID = luid;

			outAdapters.push_back(std::move(adapterInfo));
		}
    }

    nvrhi::ITexture* RendererDX12::GetCurrentBackBuffer() const
    {
        return m_rhiSwapChainBuffers[m_swapChain->GetCurrentBackBufferIndex()];
    }

    nvrhi::ITexture* RendererDX12::GetBackBuffer(u32 index) const
    {
        return (index < static_cast<u32>(m_rhiSwapChainBuffers.size())) 
            ? m_rhiSwapChainBuffers[index] : nullptr;
    }

    u32 RendererDX12::GetCurrentBackBufferIndex() const
    {
        return m_swapChain->GetCurrentBackBufferIndex();
    }

    u32 RendererDX12::GetBackBufferCount() const
    {
        return m_swapChainDesc.BufferCount;
    }

    void RendererDX12::ReportLiveObjects()
    {
        nvrhi::RefCountPtr<IDXGIDebug> debug;
        if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
        {
            RYU_LOG_TRACE("Reporting DXGI live objects");
            DXGI_DEBUG_RLO_FLAGS flags = (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_DETAIL);
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, flags);
        }

        if (m_device)
        {
            ComPtr<ID3D12DebugDevice> d3dDebugDevice;
            if (SUCCEEDED(m_device->QueryInterface(IID_PPV_ARGS(&d3dDebugDevice))))
            {
                RYU_LOG_TRACE("Valid device available, reporting DX12 live objects");
                d3dDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
            }
        }
    }

    void RendererDX12::Shutdown()
    {
        RendererBase::Shutdown();

        m_dxgiAdapter = nullptr;
        m_dxgiFactory = nullptr;

        if (m_initInfo.EnableDebugRuntime)
        {
            ReportLiveObjects();
        }
    }

    bool RendererDX12::CreateDeviceIndependentResources()
    {
        if (!m_dxgiFactory)
        {
            const u32 flags = m_initInfo.EnableDebugRuntime ? DXGI_CREATE_FACTORY_DEBUG : 0;
            if (FAILED(::CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_dxgiFactory))))
            {
                RYU_LOG_ERROR("Failed to create DXGI factory!");
                return false;
            }
        }

        return true;
    }

    bool RendererDX12::CreateDevice()
    {
        // Check for debug layer
        if (m_initInfo.EnableDebugRuntime)
        {
            nvrhi::RefCountPtr<ID3D12Debug> debug;
            if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
            {
                debug->EnableDebugLayer();
            }
            else
            {
                RYU_LOG_WARN("Unable to enable DX12 debug layer");
            }
        }

        // Check for GPU validation
        if (m_initInfo.EnableGPUValidation)
        {
            nvrhi::RefCountPtr<ID3D12Debug3> debug;
            if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
            {
                debug->SetEnableGPUBasedValidation(true);
            }
            else
            {
                RYU_LOG_WARN("Unable to enable DX12 GPU validation");
            }
        }

        u32 adapterIndex = 0;
        if (FAILED(m_dxgiFactory->EnumAdapters(adapterIndex, &m_dxgiAdapter)))
        {
            if (adapterIndex == 0)
            {
                RYU_LOG_ERROR("Cannot find any DXGI adapters in the system!");
            }
            else
            {
                RYU_LOG_ERROR("Specified DXGI adapter {} does not exist", adapterIndex);
            }

            return false;
        }

        // Cache adapter desc
        {
            DXGI_ADAPTER_DESC aDesc;
            m_dxgiAdapter->GetDesc(&aDesc);
            m_rendererString = GetAdapterName(aDesc);
        }

        if (FAILED(::D3D12CreateDevice(m_dxgiAdapter, m_initInfo.FeatureLevel, IID_PPV_ARGS(&m_device))))
        {
            RYU_LOG_ERROR("Failed to create D3D12 device!");
            return false;
        }

        // Set up message filter layer
        if (m_initInfo.EnableDebugRuntime)
        {
            nvrhi::RefCountPtr<ID3D12InfoQueue> infoQueue;
            m_device->QueryInterface(&infoQueue);

            if (infoQueue)
            {
#if defined (RYU_BUILD_DEBUG)
                if (m_initInfo.EnableWarningsAsErrors)
                {
                    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
                }
                infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
                infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif

                D3D12_MESSAGE_ID disableMessageIDs[] =
                {
                    D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
                    D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH, // descriptor validation doesn't understand acceleration structures
                    D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,                               // NGX currently generates benign resource creation warnings
                };

                D3D12_INFO_QUEUE_FILTER filter{};
                filter.DenyList.pIDList = disableMessageIDs;
                filter.DenyList.NumIDs = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[0]);
                infoQueue->AddStorageFilterEntries(&filter);
            }
        }

        // Create command queues
        D3D12_COMMAND_QUEUE_DESC queueDesc
        {
            .Type     = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = 0,
            .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 1
        };
        HR_RETURN(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsQueue)));

        if (m_initInfo.EnableComputeQueue)
        {
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            HR_RETURN(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_computeQueue)));
        }

        if (m_initInfo.EnableCopyQueue)
        {
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            HR_RETURN(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue)));
        }


        nvrhi::d3d12::DeviceDesc deviceDesc;
        deviceDesc.errorCB                   = this;
        deviceDesc.pDevice                   = m_device;
        deviceDesc.pGraphicsCommandQueue     = m_graphicsQueue;
        deviceDesc.pComputeCommandQueue      = m_computeQueue;
        deviceDesc.pCopyCommandQueue         = m_copyQueue;
        deviceDesc.logBufferLifetime         = m_initInfo.LogBufferLifetime;
        deviceDesc.enableHeapDirectlyIndexed = m_initInfo.EnableHeapDirectlyIndexed;

        m_nvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);

        if (m_initInfo.EnableNVRHIValidationLayer)
        {
            m_nvrhiDevice = nvrhi::validation::createValidationLayer(m_nvrhiDevice);
        }

        //m_featureSupport.Init(m_device.Get());

        return true;
    }

    bool RendererDX12::CreateSwapChain()
    {
        RECT clientRect;
        ::GetClientRect(m_hWnd, &clientRect);
        u32 width = clientRect.right - clientRect.left;
        u32 height = clientRect.bottom - clientRect.top;

        ZeroMemory(&m_swapChainDesc, sizeof(m_swapChainDesc));
        m_swapChainDesc.Width              = width;
        m_swapChainDesc.Height             = height;
        m_swapChainDesc.SampleDesc.Count   = m_initInfo.SwapChainSampleCount;
        m_swapChainDesc.SampleDesc.Quality = m_initInfo.SwapChainSampleQuality;
        m_swapChainDesc.BufferUsage        = m_initInfo.SwapChainUsage;
        m_swapChainDesc.BufferCount        = m_initInfo.SwapChainBufferCount;
        m_swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        m_swapChainDesc.Flags              = 0;  // NOTE: Could consider adding flag for DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

        switch (m_initInfo.SwapChainFormat)
        {
            using enum nvrhi::Format;
        case SRGBA8_UNORM: m_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                                break;
        case SBGRA8_UNORM: m_swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;                                break;
        default: m_swapChainDesc.Format = nvrhi::d3d12::convertFormat(m_initInfo.SwapChainFormat); break;
        }

        nvrhi::RefCountPtr<IDXGIFactory5> dxgiFactory5;
        if (SUCCEEDED(m_dxgiFactory->QueryInterface(IID_PPV_ARGS(&dxgiFactory5))))
        {
            BOOL supported = 0;
            if (SUCCEEDED(dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
            {
                m_isTearingSupported = (supported != 0);
            }
        }

        if (m_isTearingSupported)
        {
            m_swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        }

        m_fullScreenDesc                         = {};
        m_fullScreenDesc.RefreshRate.Numerator   = m_initInfo.RefreshRate;
        m_fullScreenDesc.RefreshRate.Denominator = 1;
        m_fullScreenDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
        m_fullScreenDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
        m_fullScreenDesc.Windowed                = TRUE;  // NOTE: Handle fullscreening eventually here
    
        nvrhi::RefCountPtr<IDXGISwapChain1> swapChain1;
        HR_RETURN(m_dxgiFactory->CreateSwapChainForHwnd(m_graphicsQueue, m_hWnd, &m_swapChainDesc, &m_fullScreenDesc, nullptr, &swapChain1));
        HR_RETURN(swapChain1->QueryInterface(IID_PPV_ARGS(&m_swapChain)));

        if (!CreateRenderTargets())
        {
            RYU_LOG_ERROR("Failed to create render targets!");
            return false;
        }

        HR_RETURN(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFence)));

        for (u32 bufferIndex = 0; bufferIndex < m_swapChainDesc.BufferCount; bufferIndex++)
        {
            m_frameFenceEvents.push_back(::CreateEvent(nullptr, FALSE, TRUE, nullptr));
        }

        return true;
    }

    void RendererDX12::DestroyDeviceAndSwapChain()
    {
        m_rhiSwapChainBuffers.clear();
        m_rendererString.clear();

        ReleaseRenderTargets();

        m_nvrhiDevice = nullptr;

        for (auto fenceEvent : m_frameFenceEvents)
        {
            ::WaitForSingleObject(fenceEvent, INFINITE);
            ::CloseHandle(fenceEvent);
        }

        m_frameFenceEvents.clear();

        if (m_swapChain)
        {
            m_swapChain->SetFullscreenState(FALSE, nullptr);
        }

        m_swapChainBuffers.clear();

        m_frameFence    = nullptr;
        m_swapChain     = nullptr;
        m_graphicsQueue = nullptr;
        m_computeQueue  = nullptr;
        m_copyQueue     = nullptr;
        m_device        = nullptr;
    }

    void RendererDX12::ResizeSwapChain()
    {
        ReleaseRenderTargets();

        if (!m_nvrhiDevice || !m_swapChain)
        {
            return;
        }

        const auto& [width, height] = m_initInfo.BackBufferSize;

        const HRESULT hr = m_swapChain->ResizeBuffers(
            m_initInfo.SwapChainBufferCount,
            width, height,
            m_swapChainDesc.Format,
            m_swapChainDesc.Flags);

        if (FAILED(hr))
        {
            RYU_LOG_FATAL("Failed to resize swapchain!");
        }

        if (!CreateRenderTargets())
        {
            RYU_LOG_FATAL("Failed to create render targets!");
        }
    }

    bool RendererDX12::BeginFrame()
    {
        // NOTE: Could handle checking if swapchain windowed state has changed to trigger resize of needed

        u32 bufferIndex = m_swapChain->GetCurrentBackBufferIndex();
        ::WaitForSingleObject(m_frameFenceEvents[bufferIndex], INFINITE);

        return true;
    }

    bool RendererDX12::Present()
    {
        u32 bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

        u32 presentFlags = 0;
        if (!m_initInfo.EnableVSync && m_fullScreenDesc.Windowed && m_isTearingSupported)
        {
            presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
        }

        HRESULT result = m_swapChain->Present(m_initInfo.EnableVSync ? 1 : 0, presentFlags);

        m_frameFence->SetEventOnCompletion(m_frameCount, m_frameFenceEvents[bufferIndex]);
        m_graphicsQueue->Signal(m_frameFence, m_frameCount);
        m_frameCount++;

        return SUCCEEDED(result);
    }

    bool RendererDX12::CreateRenderTargets()
    {
        m_swapChainBuffers.resize(m_swapChainDesc.BufferCount);
        m_rhiSwapChainBuffers.resize(m_swapChainDesc.BufferCount);

        const auto& [width, height] = m_initInfo.BackBufferSize;

        for (u32 i = 0; i < m_initInfo.SwapChainBufferCount; i++)
        {
            const HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffers[i]));
            HR_RETURN(hr);

            nvrhi::TextureDesc textureDesc;
            textureDesc.width            = width;
            textureDesc.height           = height;
            textureDesc.sampleCount      = m_initInfo.SwapChainSampleCount;
            textureDesc.sampleQuality    = m_initInfo.SwapChainSampleQuality;
            textureDesc.format           = m_initInfo.SwapChainFormat;
            textureDesc.debugName        = "SwapChainBuffer"s + std::to_string(i);
            textureDesc.isRenderTarget   = true;
            textureDesc.isUAV            = false;
            textureDesc.initialState     = nvrhi::ResourceStates::Present;
            textureDesc.keepInitialState = true;

            m_rhiSwapChainBuffers[i] = m_nvrhiDevice->createHandleForNativeTexture(
                nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_swapChainBuffers[i]), textureDesc);
        }
    }

    void RendererDX12::ReleaseRenderTargets()
    {
        if (m_nvrhiDevice)
        {
            m_nvrhiDevice->waitForIdle();
            m_nvrhiDevice->runGarbageCollection();
        }

        for (auto e : m_frameFenceEvents)
        {
            ::SetEvent(e);
        }

        m_rhiSwapChainBuffers.clear();
        m_swapChainBuffers.clear();
    }

}

#undef HR_RETURN
