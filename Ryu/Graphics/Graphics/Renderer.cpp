#include "Graphics/Renderer.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Core/CommandContext.h"
#include "Profiling/Profiling.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"
#include "Math/Math.h"
#include <expected>

namespace Ryu::Gfx
{
	struct Vertex
	{
		SM::Vector3 Position;
		SM::Vector4 Color;
	};

	RYU_LOG_DECLARE_CATEGORY(Renderer);

	Renderer::Renderer(HWND window)
	{
		RYU_PROFILE_SCOPE();

		m_device = Device::Create();
		DeviceWeakPtr parent = m_device;

		CreateRootSignature();
		CompileShaders();
		CreateVB();

		for (u32 i = 0; i < m_cmdAllocators.size(); i++)
		{
			m_cmdAllocators[i].Initialize(parent, CommandListType::Direct);
		}

		m_fenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
		m_fence.Initialize(parent, 0, FenceFlag::None);
		m_cmdQueue.Initialize(parent, CommandListType::Direct, CommandQueuePriority::Normal);
		m_cmdList.Initialize(parent, m_cmdAllocators[0], &m_pso, CommandListType::Direct);
		m_rtvHeap.Initialize(parent, DescriptorHeapType::RTV, DescriptorHeapFlags::None, FRAME_BUFFER_COUNT);

		m_swapChain.Initialize(m_device, m_cmdQueue, m_rtvHeap, window, BACK_BUFFER_FORMAT);

		WaitForGPU();
	}

	Renderer::~Renderer()
	{
		RYU_PROFILE_SCOPE();

		if (m_device)
		{
			RYU_LOG_DEBUG(LogRenderer, "Waiting for GPU");
			WaitForGPU();

			RYU_LOG_DEBUG(LogRenderer, "Destroying swapchain");
			m_swapChain.Destroy();

			m_vertexBuffer.Reset();
			m_pso.Destroy();
			m_rootSignature.Reset();
			m_fence.Destroy();
			m_rtvHeap.Destroy();
			m_cmdList.Destroy();
			m_cmdQueue.Destroy();

			for (auto& allocator : m_cmdAllocators)
			{
				allocator.Destroy();
			}

			if (m_fenceEvent)
			{
				::CloseHandle(m_fenceEvent);
			}

			RYU_LOG_DEBUG(LogRenderer, "Destroying graphics device");
			Device::Destroy(*m_device);
			RYU_ASSERT(m_device.use_count() == 1);
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

	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();

		PopulateCommandList();

		ID3D12CommandList* ppCommandLists[] = { m_cmdList.Get() };
		m_cmdQueue.Get()->ExecuteCommandLists(1, ppCommandLists);

		m_swapChain.Present();

		MoveToNextFrame();
	}

	void Renderer::OnResize(u32 width, u32 height)
	{
		RYU_PROFILE_SCOPE();

		WaitForGPU();
		m_swapChain.Resize(m_rtvHeap, width, height);
	}

	void Renderer::PopulateCommandList()
	{
		RYU_PROFILE_SCOPE();

		const u32 currentBackBufferIndex = m_swapChain.GetFrameIndex();

		CommandContext ctx(&m_cmdList, &m_cmdAllocators[currentBackBufferIndex], &m_pso);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			m_rtvHeap.GetCPUHandle(),
			currentBackBufferIndex,
			m_rtvHeap.GetDescriptorSize()
		);

		const RenderSurface& renderSurface = m_swapChain.GetRenderSurface(currentBackBufferIndex);
		auto backBuffer = renderSurface.Resource;

		// Transition to render target
		ctx.SetResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		));

		m_cmdList.Get()->SetGraphicsRootSignature(m_rootSignature.Get());
		m_cmdList.Get()->RSSetViewports(1, &m_swapChain.GetViewport());
		m_cmdList.Get()->RSSetScissorRects(1, &m_swapChain.GetScissorRect());

		// Set render target and clear
		const float clearColor[] = { 0.2f, 0.3f, 0.4f, 1.0f }; // Dark blue
		m_cmdList.Get()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_cmdList.Get()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Draw
		m_cmdList.Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_cmdList.Get()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		m_cmdList.Get()->DrawInstanced(3, 1, 0, 0);

		// Transition back to present
		ctx.SetResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		));
	}

	void Renderer::WaitForGPU()
	{
		RYU_PROFILE_SCOPE();

		DX12::Device* const device = m_device->GetDevice();
		const u32 currentBackBufferIndex = m_swapChain.GetFrameIndex();

		// Schedule a signal command in the queue
		m_cmdQueue.Signal(m_fence, m_fenceValues[currentBackBufferIndex]);

		// Wait until the fence has been crossed
		DXCallEx(m_fence.Get()->SetEventOnCompletion(m_fenceValues[currentBackBufferIndex], m_fenceEvent), device);

		if (::WaitForSingleObjectEx(m_fenceEvent, 5000, FALSE) == WAIT_TIMEOUT)
		{
			RYU_LOG_ERROR(LogRenderer, "GPU wait timed out");
		}

		// Increment the fence value for the current frame
		m_fenceValues[currentBackBufferIndex]++;
	}

	void Renderer::MoveToNextFrame()
	{
		DX12::Device* const device = m_device->GetDevice();
		const u32 currentBackBufferIndex = m_swapChain.GetFrameIndex();
		const u64 currentFenceValue = m_fenceValues[currentBackBufferIndex];

		// Schedule a signal command in the queue
		DXCallEx(m_cmdQueue.Get()->Signal(m_fence.Get(), currentFenceValue), device);

		// Update frame index
		const u32 nextFrameIndex = m_swapChain.GetFrameIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready
		if (m_fence.Get()->GetCompletedValue() < m_fenceValues[nextFrameIndex])
		{
			DXCallEx(m_fence.Get()->SetEventOnCompletion(m_fenceValues[nextFrameIndex], m_fenceEvent), device);

			if (::WaitForSingleObjectEx(m_fenceEvent, 5000, FALSE) == WAIT_TIMEOUT)
			{
				RYU_LOG_ERROR(LogRenderer, "Move to next frame timed out!");
			}
		}

		// Set the fence value for the next frame
		m_fenceValues[nextFrameIndex] = currentFenceValue + 1;
	}
}
