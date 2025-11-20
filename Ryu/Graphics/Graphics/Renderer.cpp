#include "Graphics/Renderer.h"
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Primitives/Triangle.h"

namespace Ryu::Gfx
{
	Renderer::Renderer(HWND window)
	{
		m_device = std::make_unique<Device>(window);
		m_device->Initialize();

		CreateResources();
	}

	void Renderer::CreateResources()
	{
		CreateRootSignature();
		CreateConstantBuffer();
		CompileShaders();
		CreatePipelineState();
	
		// Create vertex buffer
		Buffer::Desc vbDesc
		{
			.SizeInBytes   = Primitives::TriangleVertexBufferSize,
			.StrideInBytes = sizeof(VertexPosCol),
			.Usage         = Buffer::Usage::Default,  // GPU only memory
			.Type          = Buffer::Type::Vertex,
			.Name          = "Triangle Vertex Buffer"
		};

		m_vertexBuffer = std::make_unique<Buffer>(m_device.get(), vbDesc);
	}
	
	void Renderer::CreateRootSignature()
	{
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
		static std::string_view shaderFile = R"(Shaders\Engine\Triangle.hlsl)";
		ShaderCompileInfo info
		{
			.FilePath = shaderFile,
			.Type     = ShaderType::VertexShader,
			.Name     = "TriangleVS"
		};

		m_vs = Shader::Compile(info);

		info.Name = "TrianglePS";
		info.Type = ShaderType::PixelShader;
		m_ps      = Shader::Compile(info);

		RYU_ASSERT(m_vs.IsValid(), "Failed to compile vertex shader!");
		RYU_ASSERT(m_ps.IsValid(), "Failed to compile pixel shader!");
	}

	void Renderer::CreatePipelineState()
	{
		const D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		Shader::Blob* const vsBlob = m_vs.GetBlob();
		Shader::Blob* const psBlob = m_ps.GetBlob();

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

	static f32 t = 0;

	void Renderer::Render()
	{
		const CommandList* cmdList  = m_device->GetGraphicsCommandList();
		const Texture* renderTarget = m_device->GetCurrentBackBuffer();

		m_device->BeginFrame(m_pipelineState.get());

		cmdList->SetGraphicsRootSignature(*m_rootSignature);
		cmdList->SetDescriptorHeap(*m_cbvHeap);
		cmdList->SetGraphicsRootDescriptorTable(0, m_constantBuffer->GetDescriptorHandle());

		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_device->SetBackBufferRenderTarget(true);

		if (m_vertexBuffer->NeedsUpload())
		{
			m_vertexBuffer->UploadData(*cmdList, Primitives::TriangleVerticesPosCol.data());
		}

		if (ConstantBuffer* mappedData = m_constantBuffer->Map<ConstantBuffer>())
		{
			t += 0.05f;
			m_cbData.OffsetX = std::sinf(t);
			m_cbData.OffsetY = std::cosf(t);

			std::memcpy(mappedData, &m_cbData, sizeof(ConstantBuffer));
		}

		cmdList->SetVertexBuffer(0, *m_vertexBuffer);		
		cmdList->SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->DrawInstanced(3, 1, 0, 0);

		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		m_device->EndFrame();
		m_device->Present();
	}
	
	void Renderer::OnResize(u32 w, u32 h)
	{
		m_device->ResizeBuffers(w, h);
	}
}
