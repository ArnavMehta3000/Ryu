#include "Renderer.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"
#include "Utils/StringConv.h"
#include "Graphics/Core/Debug/DebugLayer.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Math/Math.h"
#include <DirectXColors.h>

namespace Ryu::Gfx
{
	const bool g_enableDebugLayer = Globals::g_isDebug;
	const bool g_enableValidation = true;
	const bool g_useWarpDevice = false;
	const bool g_allowTearing = true;
	const bool g_isVsync = true;
	const DXGI_FORMAT g_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	Renderer::Renderer(HWND window)
		: m_hWnd(window)
	{
		// Get window size
		[](HWND hWNd, u32& outWidth, u32& outHeight) -> void
		{
			RECT rc{};
			::GetClientRect(hWNd, &rc);
			outWidth = static_cast<u32>(rc.right - rc.left);
			outHeight = static_cast<u32>(rc.bottom - rc.top);
		}(m_hWnd, m_width, m_height);

		// Create DXGI factory
		u32 dxgiFactoryFlags = 0;

#if defined(RYU_BUILD_DEBUG)
		DebugLayer::Initialize();
		if (g_enableDebugLayer)
		{
			RYU_LOG_TRACE("Creating DX12 device with debug layer enabled");
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		DXCall(::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

		CreateDevice();
		CreateCommandQueue();
		CreateSwapChain();
		CreateDescriptorHeaps();
		CreateFrameResources();
		CreatePipelineState();
		CreateCmdList();
		CreateVertexBuffer();
		CreateSynchronization();

		WaitForPreviousFrame();
	}

	Renderer::~Renderer()
	{
		WaitForPreviousFrame();

		if (m_fenceEvent)
		{
			::CloseHandle(m_fenceEvent);
		}

		ComRelease(m_factory);
		ComRelease(m_swapChain);

		for (auto& rt : m_renderTargets)
		{
			ComRelease(rt);
		}

		ComRelease(m_vertexBuffer);
		ComRelease(m_rootSig);
		ComRelease(m_pipelineState);
		ComRelease(m_rtvHeap);
		ComRelease(m_cmdList);
		ComRelease(m_cmdQueue);
		ComRelease(m_cmdAllocator);
		ComRelease(m_fence);

		RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(m_device, false));  // Stops hard crash when reporting live objects
		RYU_DEBUG_OP(DebugLayer::Shutdown());  // Reports DXGI
		RYU_DEBUG_OP(DebugLayer::ReportLiveDeviceObjectsAndReleaseDevice(m_device));
		
	}
	
	void Renderer::Render()
	{
		DXCall(m_cmdAllocator->Reset());
		DXCall(m_cmdList->Reset(m_cmdAllocator.Get(), m_pipelineState.Get()));

		m_cmdList->SetGraphicsRootSignature(m_rootSig.Get());
		m_cmdList->RSSetViewports(1, &m_viewport);
		m_cmdList->RSSetScissorRects(1, &m_scissorRect);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_frameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_cmdList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_descriptorSize);
		m_cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// --- Record commands ---
		m_cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::CornflowerBlue, 0, nullptr);
		m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		m_cmdList->DrawInstanced(3, 1, 0, 0);


		barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_frameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		m_cmdList->ResourceBarrier(1, &barrier);

		DXCall(m_cmdList->Close());

		// -------------------- Command list populated, now render

		ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
		m_cmdQueue->ExecuteCommandLists(1, cmdLists);

		// Not dowing tearing/VSYNC compatibility checks here
		DXCall(m_swapChain->Present(g_isVsync ? 1 : 0, 0));

		WaitForPreviousFrame();
	}
	
	void Renderer::OnResize(u32 w, u32 h)
	{
		RYU_LOG_TRACE("Renderer begin resize");

		m_width = w;
		m_height = h;

		WaitForPreviousFrame();
		
		// Release all references to swap chain buffers
		for (auto& rt : m_renderTargets)
		{
			ComRelease(rt);
		}

		DXGI_SWAP_CHAIN_DESC1 desc;
		m_swapChain->GetDesc1(&desc);

		DXCall(m_swapChain->ResizeBuffers(
			FRAME_BUFFER_COUNT,
			m_width,
			m_height,
			desc.Format,
			desc.Flags
		));

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (f32)m_width, (f32)m_height);
		m_scissorRect = CD3DX12_RECT(0, 0, m_width, m_height);

		CreateFrameResources();

		RYU_LOG_DEBUG("Renderer resized {}x{}", w, h);
	}
	
	void Renderer::CreateDevice()
	{
		if (g_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			DXCall(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

			DXCall(::D3D12CreateDevice(
				warpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_device)
			));
		}
		else
		{
			ComPtr<DXGI::Adapter> adapter;
			for (u32 adapterIndex = 0;
				SUCCEEDED(m_factory->EnumAdapterByGpuPreference(
					adapterIndex,
					DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
					IID_PPV_ARGS(&adapter)));
				adapterIndex++)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
				{
					continue;  // Don't select the Basic Render Driver adapter.
				}

				if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					const std::string description = Utils::ToNarrowStr(desc.Description);
					RYU_LOG_INFO("Using GPU: {} - {:.2f} GB", description, Math::BytesToGB(desc.DedicatedVideoMemory));
					break;
				}
			}

			RYU_ASSERT(adapter, "Failed to find a suitable GPU adapter!");

			constexpr std::array featureLevels =
			{
				D3D_FEATURE_LEVEL_12_2,
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0
			};

			// Create device
			DXCall(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
			
			D3D12_FEATURE_DATA_FEATURE_LEVELS caps{};
			caps.pFeatureLevelsRequested = featureLevels.data();
			caps.NumFeatureLevels        = (u32)featureLevels.size();

			DXCall(m_device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &caps, sizeof(caps)));
			DXCall(::D3D12CreateDevice(adapter.Get(), caps.MaxSupportedFeatureLevel, IID_PPV_ARGS(&m_device)));
		
			RYU_DEBUG_OP(DebugLayer::SetupSeverityBreaks(m_device, true));
			//RYU_DEBUG_OP(DebugLayer::SetStablePowerState(m_device, true));

			DX12::SetObjectName(m_device.Get(), "Main Device");
		}
	}
	
	void Renderer::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC desc
		{
			.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		DXCall(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_cmdQueue)));
		DX12::SetObjectName(m_cmdQueue.Get(), "Graphics Command Queue");

		// Create command allocator
		DXCall(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)));
	}
	
	void Renderer::CreateSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc{};
		scDesc.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;
		scDesc.BufferCount        = FRAME_BUFFER_COUNT;
		scDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Format             = g_backBufferFormat;
		scDesc.Width              = m_width;
		scDesc.Height             = m_height;
		scDesc.Scaling            = DXGI_SCALING_NONE;
		scDesc.Stereo             = FALSE;
		scDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.SampleDesc.Count   = 1;
		scDesc.SampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc
		{
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED,
			.Windowed         = TRUE
		};

		ComPtr<IDXGISwapChain1> swapChain1;
		DXCall(m_factory->CreateSwapChainForHwnd(
			m_cmdQueue.Get(),
			m_hWnd,
			&scDesc,
			&fsDesc,
			nullptr,
			&swapChain1));

		m_swapChain.Reset();
		DXCall(swapChain1.As(&m_swapChain));
		
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (f32)m_width, (f32)m_height);
		m_scissorRect = CD3DX12_RECT(0, 0, m_width, m_height);
	}

	void Renderer::CreateDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = FRAME_BUFFER_COUNT,
			.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask       = 0
		};

		DXCall(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap)));
		DX12::SetObjectName(m_rtvHeap.Get(), "RTV Heap");

		m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Renderer::CreateFrameResources()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		static constexpr std::array objectNames = { "Backbuffer 0", "Backbuffer 1", "Backbuffer 2", "Backbuffer 3" };
		static_assert(FRAME_BUFFER_COUNT <= objectNames.size());  // We can have more names, but not less

		for (u32 i = 0; i < m_renderTargets.size(); i++)
		{
			DXCall(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));

			m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
			DX12::SetObjectName(m_renderTargets[i].Get(), objectNames[i]);

			rtvHandle.Offset(1, m_descriptorSize);
		}
	}
	
	void Renderer::CreateCmdList()
	{
		DXCall(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_cmdList)));
		DX12::SetObjectName(m_cmdList.Get(), "Graphics Command List");
		m_cmdList->Close();
	}
	
	void Renderer::CreateSynchronization()
	{
		DXCall(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		DX12::SetObjectName(m_fence.Get(), "Frame Fence");
		m_fenceValue = 1;

		m_fenceEvent = ::CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
	}

	void Renderer::CreatePipelineState()
	{
		// Create root signature
		CD3DX12_ROOT_SIGNATURE_DESC desc{};
		desc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		DXCall(::D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		DXCall(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSig)));
		
		DX12::SetObjectName(m_rootSig.Get(), "Root Signature");
		
		// Compile shaders
		static std::string_view shaderFile = R"(Shaders\Engine\Triangle.hlsl)";

		ShaderCompiler compiler;
		ShaderCompileInfo info
		{
			.FilePath = shaderFile,
			.Type     = ShaderType::VertexShader,
			.Name     = "TriangleVS"
		};

		ComPtr<IDxcBlob> vsBlob;
		ComPtr<IDxcBlob> psBlob;

		// Compile VS
		if (auto vsResult = compiler.Compile(info))
		{
			ShaderCompileResult& result = vsResult.value();
			vsBlob = result.ShaderBlob;
		}

		// Compile PS
		info.Name = "TrianglePS";
		info.Type = ShaderType::PixelShader;
		if (auto psResult = compiler.Compile(info))
		{
			ShaderCompileResult& result = psResult.value();
			psBlob = result.ShaderBlob;
		}
		
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.InputLayout                        = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature                     = m_rootSig.Get();
		psoDesc.VS                                 = { reinterpret_cast<byte*>(vsBlob->GetBufferPointer()), vsBlob->GetBufferSize() };
		psoDesc.PS                                 = { reinterpret_cast<byte*>(psBlob->GetBufferPointer()), psBlob->GetBufferSize() };
		psoDesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable      = FALSE;
		psoDesc.DepthStencilState.StencilEnable    = FALSE;
		psoDesc.SampleMask                         = UINT_MAX;
		psoDesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets                   = 1;
		psoDesc.RTVFormats[0]                      = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count                   = 1;

		DXCall(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));

		DX12::SetObjectName(m_pipelineState.Get(), "Graphics Pipeline State");
	}

	void Renderer::CreateVertexBuffer()
	{
		struct Vertex
		{
			Math::Vector3 Position;
			Math::Vector4 Color;
		};

		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f, 0.0f    }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f, 0.0f  }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const u32 vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled over.

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

		DXCall(m_device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, // Clear value
			IID_PPV_ARGS(&m_vertexBuffer)
		));

		DX12::SetObjectName(m_vertexBuffer.Get(), "Vertex Buffer");

		byte* vertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);  // We are not reading this resource from the GPU

		DXCall(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataBegin)));
		std::memcpy(vertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffer->Unmap(0, nullptr);

		// Set up vertex buffer view
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes  = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes    = vertexBufferSize;
	}
	
	void Renderer::WaitForPreviousFrame()
	{
		// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
		// This is code implemented as such for simplicity.

		// Signal and increment fence value
		const u64 fence = m_fenceValue;
		DXCall(m_cmdQueue->Signal(m_fence.Get(), fence));
		m_fenceValue++;

		// Wait until the previous frame is finished
		if (m_fence->GetCompletedValue() < fence)
		{
			DXCall(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
			::WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		// Increment the frame index
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
}
