#include "Graphics/Renderer.h"
#include "Graphics/IRendererHook.h"
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Primitives/Cube.h"
#include "Profiling/Profiling.h"

namespace Ryu::Gfx
{
	Renderer::Renderer(HWND window, IRendererHook* hook)
		: m_hook(hook)
		, m_shaderLibrary("./Shaders/Compiled")
	{
		RYU_PROFILE_SCOPE();

		m_device = std::make_unique<Device>(window);
		m_device->Initialize();

		const auto [w, h] = m_device->GetClientSize();
		m_camera.SetViewportSize((f32)w, (f32)h);


		CreateResources();

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

	void Renderer::CreateResources()
	{
		RYU_PROFILE_SCOPE();

		CreateRootSignature();
		CreateConstantBuffer();
		CompileShaders();
		CreatePipelineState();
		CreateMeshBuffers();
	}
	
	void Renderer::CreateRootSignature()
	{
		RYU_PROFILE_SCOPE();

		// Create a single descriptor table of CBV's
		CD3DX12_DESCRIPTOR_RANGE cbvTable{};
		cbvTable.Init(
			D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			1,  // Number of descriptors in table
			0   // Base shader register arguments are bound to for this root paramter
		);

		std::array<CD3DX12_ROOT_PARAMETER, 1> slotRootParameter{};

		slotRootParameter[0].InitAsDescriptorTable(
			1, // Number of ranges
			&cbvTable);

		CD3DX12_ROOT_SIGNATURE_DESC rsdesc{};
		rsdesc.Init(
			static_cast<u32>(slotRootParameter.size()),
			slotRootParameter.data(),
			0, nullptr,  // Samplers
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		
		m_rootSignature = std::make_unique<RootSignature>(m_device.get(), rsdesc, "Root Signature");
	}

	void Renderer::CompileShaders()
	{
		RYU_PROFILE_SCOPE();

		if (Shader* vs = m_shaderLibrary.GetShader("CubeVS"))
		{
			m_vs = vs;
		}

		if (Shader* ps = m_shaderLibrary.GetShader("CubePS"))
		{
			m_ps = ps;
		}

		RYU_ASSERT(m_vs->IsValid(), "Failed to compile vertex shader!");
		RYU_ASSERT(m_ps->IsValid(), "Failed to compile pixel shader!");
	}

	void Renderer::CreatePipelineState()
	{
		RYU_PROFILE_SCOPE();

		const D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		Shader::Blob* const vsBlob = m_vs->GetBlob();
		Shader::Blob* const psBlob = m_ps->GetBlob();

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
			CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
		};

		PipelineStateStream psoStream{};
		psoStream.RootSignature     = m_rootSignature->GetNative();
		psoStream.InputLayout       = { inputElementDescs, static_cast<u32>(std::size(inputElementDescs)) };
		psoStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoStream.VS                = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
		psoStream.PS                = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());
		psoStream.Rasterizer        = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoStream.BlendDesc         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoStream.RTVFormats        =
		{
			.RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
			.NumRenderTargets = 1
		};

		const D3D12_PIPELINE_STATE_STREAM_DESC psoStreamDesc
		{
			.SizeInBytes = sizeof(PipelineStateStream),
			.pPipelineStateSubobjectStream = &psoStream
		};

		m_pipelineState = std::make_unique<PipelineState>(m_device.get(), psoStreamDesc, "Graphics Pipeline State");
	}

	void Renderer::CreateConstantBuffer()
	{
		RYU_PROFILE_SCOPE();

		m_cbvHeap = std::make_unique<DescriptorHeap>(m_device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true, "Frame CBV Heap");
		DescriptorHandle handle = m_cbvHeap->Allocate();
		
		Buffer::Desc cbDesc
		{
			.SizeInBytes   = sizeof(ConstantBuffer),
			.Usage         = Buffer::Usage::Upload,
			.Type          = Buffer::Type::Constant,
			.Name          = "Triangle Constant Buffer"
		};

		// Since we are passing the allocated heap handle, it will also create the constant buffer view
		m_constantBuffer = std::make_unique<Buffer>(m_device.get(), cbDesc, handle);
	}

	void Renderer::CreateMeshBuffers()
	{
		// Create vertex buffer
		Buffer::Desc vbDesc
		{
			.SizeInBytes   = Primitives::CubeVertexBufferSize,
			.StrideInBytes = sizeof(VertexPosCol),
			.Usage         = Buffer::Usage::Default,  // GPU only memory
			.Type          = Buffer::Type::Vertex,
			.Name          = "Cube Vertex Buffer"
		};
		m_vertexBuffer = std::make_unique<Buffer>(m_device.get(), vbDesc);

		// Create the index buffer
		Buffer::Desc ibDesc
		{
			.SizeInBytes   = Primitives::CubeIndexBufferSize,
			.Usage         = Buffer::Usage::Default,  // GPU only memory
			.Type          = Buffer::Type::Index,
			.Format        = DXGI_FORMAT_R16_UINT,
			.Name          = "Cube Index Buffer"
		};
		m_indexBuffer = std::make_unique<Buffer>(m_device.get(), ibDesc);
	}

	static f32 t = 0;

	void Renderer::Render()
	{
		RYU_PROFILE_SCOPE();

		m_camera.Update(0.1f);  // Fake deltaTime

		CommandList* cmdList  = m_device->GetGraphicsCommandList();
		const Texture* renderTarget = m_device->GetCurrentBackBuffer();

		m_device->BeginFrame(m_pipelineState.get());

		cmdList->SetGraphicsRootSignature(*m_rootSignature);
		cmdList->SetDescriptorHeap(*m_cbvHeap);
		cmdList->SetGraphicsRootDescriptorTable(0, m_constantBuffer->GetDescriptorHandle());

		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_device->SetBackBufferRenderTarget(true);

		if (m_vertexBuffer->NeedsUpload())
		{
			m_vertexBuffer->UploadData(*cmdList, Primitives::CubeVerticesPosCol.data());
		}

		if (m_indexBuffer->NeedsUpload())
		{
			m_indexBuffer->UploadData(*cmdList, Primitives::CubeIndices.data());
		}

		if (ConstantBuffer* mappedData = m_constantBuffer->Map<ConstantBuffer>())
		{
			t += 0.1f; 
			const f32 rad = DirectX::XMConvertToRadians(t);

			const Math::Matrix scale = Math::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
			
			Math::Matrix rotation = Math::Matrix::CreateRotationX(rad)
				* Math::Matrix::CreateRotationY(rad)
				* Math::Matrix::CreateRotationZ(rad);

			Math::Matrix translation = Math::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
			Math::Matrix world = scale * rotation * translation;

			m_cbData.WVP = (world * m_camera.GetViewProjectionMatrix()).Transpose();
			std::memcpy(mappedData, &m_cbData, sizeof(ConstantBuffer));
		}

		cmdList->SetVertexBuffer(0, *m_vertexBuffer);		
		cmdList->SetIndexBuffer(*m_indexBuffer);
		cmdList->SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->GetNative()->DrawIndexedInstanced((u32)Primitives::CubeIndices.size(), 1, 0, 0, 0);

#if defined(RYU_WITH_EDITOR)
		if (m_hook)
		{
			m_hook->OnImGuiFrameBegin();
			m_hook->OnImGuiRender();
			m_hook->OnImGuiFrameEnd(cmdList);
		}
#endif

		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		m_device->EndFrame();
		m_device->Present();
	}
	
	void Renderer::OnResize(u32 w, u32 h)
	{
		RYU_PROFILE_SCOPE();
		m_device->ResizeBuffers(w, h);
		m_camera.SetViewportSize((f32)w, (f32)h);
	}
}
