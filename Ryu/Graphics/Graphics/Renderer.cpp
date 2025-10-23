#include "Graphics/Renderer.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Core/GfxPipelineStateBuilder.h"
#include "Common/Assert.h"
#include "Globals/Globals.h"
#include "Math/Math.h"
#include <DirectXColors.h>

namespace Ryu::Gfx
{
	Renderer::Renderer(HWND window)
		: m_hWnd(window)
	{
		// Get window size
		[](HWND hWnd, u32& outWidth, u32& outHeight) -> void
		{
			RECT rc{};
			::GetClientRect(hWnd, &rc);
			outWidth = static_cast<u32>(rc.right - rc.left);
			outHeight = static_cast<u32>(rc.bottom - rc.top);
		}(m_hWnd, m_width, m_height);


		// Create device with configuration
		GfxDeviceConfig deviceConfig
		{
			.EnableDebugLayer    = Globals::g_isDebug,
			.EnableValidation    = true,
			.UseWarpDevice       = false,
			.SetStablePowerState = false,
			.MinFeatureLevel     = D3D_FEATURE_LEVEL_11_0
		};
		m_device = std::make_unique<GfxDevice>(deviceConfig);

		// Create command context
		m_context = std::make_unique<CommandContext>(m_device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

		// Create swap chain
		SwapChainConfig swapChainConfig
		{
			.Width        = m_width,
			.Height       = m_height,
			.WindowHandle = m_hWnd,
			.Format       = DXGI_FORMAT_R8G8B8A8_UNORM,
			.BufferCount  = FRAME_BUFFER_COUNT,
			.AllowTearing = true,
			.EnableVSync  = true
		};
		m_swapChain = std::make_unique<GfxSwapChain>(m_device.get(), m_context.get(), swapChainConfig);

		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(m_width), static_cast<f32>(m_height));
		m_scissorRect = CD3DX12_RECT(0, 0, m_width, m_height);

		// Create rendering resources
		CreatePipelineState();
		CreateVertexBuffer();

		// Wait for initialization to complete
		m_context->WaitForGPU();

		RYU_LOG_INFO("Renderer initialized successfully");
	}

	Renderer::~Renderer()
	{
		m_context->WaitForGPU();

		m_vertexBuffer.reset();
		ComRelease(m_pipelineState);
		ComRelease(m_rootSig);

		m_swapChain.reset();
		m_context.reset();
		m_device.reset();
	}

	void Renderer::Render()
	{
		const u32 frameIndex = m_swapChain->GetCurrentFrameIndex();

		// Begin frame
		m_context->BeginFrame(frameIndex, m_pipelineState.Get());

		auto* cmdList = m_context->GetCommandList();

		// Set rendering state
		cmdList->SetGraphicsRootSignature(m_rootSig.Get());
		cmdList->RSSetViewports(1, &m_viewport);
		cmdList->RSSetScissorRects(1, &m_scissorRect);

		// Transition backbuffer to render target
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_swapChain->GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdList->ResourceBarrier(1, &barrier);

		// Get RTV handle
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain->GetCurrentRTV();
		cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Clear and draw
		cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::CornflowerBlue, 0, nullptr);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto vbView = m_vertexBuffer->GetVertexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vbView);
		cmdList->DrawInstanced(3, 1, 0, 0);

		// Transition backbuffer to present
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_swapChain->GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);

		// End frame and execute
		m_context->EndFrame();
		m_context->ExecuteCommandList();

		// Present
		m_swapChain->Present();

		// Move to next frame
		m_context->MoveToNextFrame(m_swapChain->GetCurrentFrameIndex());
	}

	void Renderer::OnResize(u32 w, u32 h)
	{
		RYU_LOG_TRACE("Renderer begin resize");

		m_width = w;
		m_height = h;

		// Resize swap chain (handles GPU wait internally)
		m_swapChain->Resize(m_width, m_height);

		// Update viewport and scissor rect
		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(m_width), static_cast<f32>(m_height));
		m_scissorRect = CD3DX12_RECT(0, 0, m_width, m_height);

		RYU_LOG_DEBUG("Renderer resized {}x{}", w, h);
	}

	void Renderer::CreatePipelineState()
	{
		// Create root signature
		CD3DX12_ROOT_SIGNATURE_DESC desc{};
		desc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		DXCall(::D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		DXCall(m_device->GetDevice()->CreateRootSignature(
			0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSig)));

		DX12::SetObjectName(m_rootSig.Get(), "Root Signature");

		// Compile shaders
		static std::string_view shaderFile = R"(Shaders\Engine\Triangle.hlsl)";

		ShaderCompiler compiler;
		ShaderCompileInfo info
		{
			.FilePath = shaderFile,
			.Type = ShaderType::VertexShader,
			.Name = "TriangleVS"
		};

		ComPtr<IDxcBlob> vsBlob;
		ComPtr<IDxcBlob> psBlob;

		// Compile VS
		if (auto vsResult = compiler.Compile(info))
		{
			vsBlob = vsResult.value().ShaderBlob;
		}

		// Compile PS
		info.Name = "TrianglePS";
		info.Type = ShaderType::PixelShader;
		if (auto psResult = compiler.Compile(info))
		{
			psBlob = psResult.value().ShaderBlob;
		}

		// Input layout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Create PSO
		m_pipelineState = GfxPipelineStateBuilder::CreateSimple(
			m_device.get(),
			m_rootSig.Get(),
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			psBlob->GetBufferPointer(),
			psBlob->GetBufferSize(),
			&inputElementDescs[0], 2,
			m_swapChain->GetFormat(),
			"Graphics Pipeline State"
		);
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
			{ { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		BufferDesc bufferDesc
		{
			.SizeInBytes   = sizeof(triangleVertices),
			.StrideInBytes = sizeof(Vertex),
			.Type          = BufferType::Vertex,
			.HeapType      = D3D12_HEAP_TYPE_UPLOAD,
			.InitialState  = D3D12_RESOURCE_STATE_GENERIC_READ,
			.InitialData   = triangleVertices,
			.DebugName     = "Triangle Vertex Buffer"
		};

		m_vertexBuffer = std::make_unique<GfxBuffer>(m_device.get(), bufferDesc);
	}
}
