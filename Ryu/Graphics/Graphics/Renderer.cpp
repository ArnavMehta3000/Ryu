#include "Graphics/Renderer.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Core/CommandContext.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"
#include "Config/CmdLine.h"
#include "Profiling/Profiling.h"
#include "Logging/Logger.h"
#include "Math/Math.h"
#include <expected>

namespace Ryu::Gfx
{
	static Config::CVar<bool> cv_debugLayerEnabled(
		"Gfx.EnableDebugLayer",
		Globals::g_isDebug,
		"Enable graphics debug layer. Enabled by default in debug builds",
		Config::CVarFlags::Debug | Config::CVarFlags::ReadOnly);

	static Config::CVar<bool> cv_enableValidation(
		"Gfx.EnableValidation",
		false,
		"Enable GPU based validation. Off by default",
		Config::CVarFlags::Debug | Config::CVarFlags::ReadOnly);

	static Config::CVar<i32> cv_syncInterval(
		"Gfx.SyncInterval",
		0,
		"Swapchain sync interval. 0: no sync (default) | 1: vsync enabled");

	static Config::CVar<bool> cv_allowTearing(
		"Gfx.AllowTearing",
		true,
		"Allow screen tearing if supported. Note vsync and tearing cannot be enabled at the same time");

	static Config::CVar<bool> cv_useWarp(
		"Gfx.UseWARP",
		false,
		"Use WARP graphics device (Default: false)");
	

	bool IsDebugLayerEnabled() { return cv_debugLayerEnabled.Get(); }
	bool IsGPUBasedValidationEnabled() { return cv_enableValidation.Get(); }
	bool IsTearingAllowed() { return cv_allowTearing.Get(); }
	i32 GetSyncInterval() { return cv_syncInterval.Get(); }
	bool ShouldUseWARPDevice() { return cv_useWarp.Get(); }

	struct Vertex
	{
		SM::Vector3 Position;
		SM::Vector4 Color;
	};

	RYU_LOG_DECLARE_CATEGORY(Renderer);

	Renderer::Renderer(HWND window)
		: m_window(window)
	{
		RYU_ASSERT(m_window, "Window is not initialized.");
	}

	void Renderer::Initialize()
	{
		RYU_PROFILE_SCOPE();

		m_device = Device::Create();
		DeviceWeakPtr parent = m_device;

		CreateRootSignature();
		CompileShaders();
		CreateVB();

		bool result = true;
		static_cast<DeviceObject<DescriptorHeap>*>(&m_rtvDescHeap)->Initialize(parent, DescriptorHeapType::RTV);
		static_cast<DeviceObject<DescriptorHeap>*>(&m_dsvDescHeap)->Initialize(parent, DescriptorHeapType::DSV);
		static_cast<DeviceObject<DescriptorHeap>*>(&m_srvDescHeap)->Initialize(parent, DescriptorHeapType::CBV_SRV_UAV);
		static_cast<DeviceObject<DescriptorHeap>*>(&m_uavDescHeap)->Initialize(parent, DescriptorHeapType::CBV_SRV_UAV);
		result &= m_rtvDescHeap.Initialize(512, false);
		result &= m_dsvDescHeap.Initialize(512, false);
		result &= m_srvDescHeap.Initialize(4096, true);
		result &= m_uavDescHeap.Initialize(512, false);

		if (!result)
		{
			RYU_LOG_FATAL(LogRenderer, "Failed to create descriptor heaps");
		}

		auto& cmdQueue = m_device->GetCommandContext().GetCommandQueue();
		m_swapChain.Initialize(m_device, cmdQueue, m_rtvDescHeap, m_window, BACK_BUFFER_FORMAT);

#if defined(RYU_WITH_EDITOR)
		m_imguiRenderer = std::make_unique<ImGuiRenderer>();
		m_imguiRenderer->Initialize(m_device.get(), m_window, m_srvDescHeap, FRAME_BUFFER_COUNT, m_swapChain.GetFormat());
#endif
		// Wait until initialization is complete
		m_device->GetCommandContext().Flush();
	}

	void Renderer::Shutdown()
	{
		RYU_PROFILE_SCOPE();

		if (m_device)
		{
			m_device->GetCommandContext().Flush();

#if defined(RYU_WITH_EDITOR)
			m_imguiRenderer->Shutdown();
			m_imguiCallback.reset();
			m_imguiRenderer.reset();
#endif

			RYU_LOG_DEBUG(LogRenderer, "Destroying swapchain");
			m_swapChain.Destroy();

			m_vertexBuffer.Reset();
			m_pso.Destroy();
			m_rootSignature.Reset();

			for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
			{
				ProcessDeferredReleases(i);
			}

			m_rtvDescHeap.Release();
			m_dsvDescHeap.Release();
			m_srvDescHeap.Release();
			m_uavDescHeap.Release();

			for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
			{
				ProcessDeferredReleases(i);
			}

			RYU_LOG_DEBUG(LogRenderer, "Destroying graphics device");
			Device::Destroy(*m_device);
			auto useCount = m_device.use_count();
			RYU_ASSERT(useCount == 1);
			m_device.reset();
		}
	}

	void Renderer::CreateRootSignature()
	{
		// Create empty root signature
		{
			CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
			rsDesc.Init(
				0,                                                            // Num Parameters
				nullptr,                                                      // Root params
				0,                                                            // Num static samplers
				nullptr,                                                      // Static samplers
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT  // Flags
			);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			DXCall(D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
			DXCallEx(m_device->GetDevice()->CreateRootSignature(
				0,
				signature->GetBufferPointer(),
				signature->GetBufferSize(),
				IID_PPV_ARGS(&m_rootSignature)),
			m_device->GetDevice());
		}
	}

	void Renderer::CompileShaders()
	{
		ShaderCompileInfo vsInfo
		{
			.FilePath = L"Shaders/Engine/Triangle.hlsl",
			.Type = ShaderType::VertexShader,
			.Name = "TriangleVS",
		};

		ShaderCompileInfo psInfo = vsInfo;
		psInfo.Type = ShaderType::PixelShader;
		psInfo.Name = "TrianglePS";

		ShaderCompileResult vsResult, psResult;

		ShaderCompiler compiler;
		if (auto result = compiler.Compile(vsInfo))
		{
			vsResult = std::move(result.value());
		}
		else
		{
			RYU_LOG_ERROR(LogRenderer, "Shader Compiler Error: {}", result.error());
		}

		if (auto result = compiler.Compile(psInfo))
		{
			psResult = std::move(result.value());
		}
		else
		{
			RYU_LOG_ERROR(LogRenderer, "Shader Compiler Error: {}", result.error());
		}


		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Create PSO
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.InputLayout                     = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature                  = m_rootSignature.Get();
		psoDesc.VS                              = CD3DX12_SHADER_BYTECODE(vsResult.ShaderBlob->GetBufferPointer(), vsResult.ShaderBlob->GetBufferSize());
		psoDesc.PS                              = CD3DX12_SHADER_BYTECODE(psResult.ShaderBlob->GetBufferPointer(), psResult.ShaderBlob->GetBufferSize());
		psoDesc.RasterizerState                 = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState                      = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable   = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask                      = UINT_MAX;
		psoDesc.PrimitiveTopologyType           = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets                = 1;
		psoDesc.RTVFormats[0]                   = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count                = 1;

		m_pso.Initialize(m_device, psoDesc);
	}

	void Renderer::CreateVB()
	{
		std::array<Vertex, 3> vertices =
		{
			Vertex{.Position = {0.0f, 0.25f, 0.0f}, .Color = {1.0f, 0.0f, 0.0f, 1.0f} }, // Top
			Vertex{.Position = {0.25f, -0.25f, 0.0f}, .Color = {0.0f, 1.0f, 0.0f, 1.0f} }, // Right
			Vertex{.Position = {-0.25f, -0.25f, 0.0f}, .Color = {0.0f, 0.0f, 1.0f, 1.0f} }, // Left
		};

		constexpr auto vbSize = sizeof(vertices);
		auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

		// This creates the vertex buffer, but it has not been uploaded to the GPU.
		RYU_TODO("Move to upload heap. This is not reccomended.");
		DXCall(m_device->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)
		));

		// Copy the triangle data to the vertex buffer.
		u8* vertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		DXCall(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataBegin)));
		std::memcpy(vertexDataBegin, vertices.data(), vbSize);
		m_vertexBuffer->Unmap(0, nullptr);
		DX12::SetObjectName(m_vertexBuffer.Get(), "Vertex Buffer");

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = vbSize;

	}

	void Renderer::ProcessDeferredReleases(u32 frameIndex)
	{
		// TODO: Lock using mutex
		m_rtvDescHeap.ProcessDeferredFree(frameIndex);
		m_dsvDescHeap.ProcessDeferredFree(frameIndex);
		m_srvDescHeap.ProcessDeferredFree(frameIndex);
		m_uavDescHeap.ProcessDeferredFree(frameIndex);

		m_device->ProcessDeferredReleases(frameIndex);
	}

	void Renderer::DeferredRelease(IUnknown* resource)
	{
		m_device->DeferredRelease(resource);
	}

	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();
		
		m_device->BeginFrame(&m_pso);  // This will process deferred releases

		PopulateCommandList();	

		m_device->EndFrame();

		m_swapChain.Present();
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();

		const CD3DX12_VIEWPORT& vp = m_swapChain.GetViewport();

		if (vp.Width == width && vp.Height == height)
		{
			return;  // Size is the same
		}

		// Wait for all GPU work to complete
		m_device->GetCommandContext().Flush();

		m_swapChain.Resize();
	}

	void Renderer::PopulateCommandList()
	{
		RYU_PROFILE_SCOPE();

		const u32 currentBackBufferIndex = m_swapChain.GetFrameIndex();

		auto& ctx = m_device->GetCommandContext();
		auto& cmdList = ctx.GetCommandList();

		const RenderSurface& renderSurface = m_swapChain.GetRenderSurface(currentBackBufferIndex);

		// Transition to render target
		ctx.SetResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			renderSurface.Resource.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		));

		cmdList.Get()->SetGraphicsRootSignature(m_rootSignature.Get());
		cmdList.Get()->RSSetViewports(1, &m_swapChain.GetViewport());
		cmdList.Get()->RSSetScissorRects(1, &m_swapChain.GetScissorRect());

		auto rtvHandle = m_rtvDescHeap.GetCPUStart();
		rtvHandle.Offset(currentBackBufferIndex, m_rtvDescHeap.GetDescriptorSize());

		// Set render target and clear
		const float clearColor[] = { 0.2f, 0.3f, 0.4f, 1.0f }; // Dark blue
		cmdList.Get()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		cmdList.Get()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Draw
		cmdList.Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList.Get()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		cmdList.Get()->DrawInstanced(3, 1, 0, 0);

#if defined(RYU_WITH_EDITOR)
		// Only worth rendering ImGui if the callback is set
		if (m_imguiCallback.has_value() && m_imguiRenderer)
		{		
			m_imguiRenderer->BeginFrame();
			(*m_imguiCallback)(this);

			// IMPORTANT: Set descriptor heap for ImGui before rendering
			ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvDescHeap.Get() };
			cmdList.Get()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

			m_imguiRenderer->Render(cmdList);
		}
#endif

		// Transition back to present
		ctx.SetResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			renderSurface.Resource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		));
	}
}
