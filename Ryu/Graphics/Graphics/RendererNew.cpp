#include "Graphics/RendererNew.h"
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Primitives/Triangle.h"

namespace Ryu::Gfx
{
	RendererNew::RendererNew(HWND window)
	{
		m_device = std::make_unique<Device>(window);
		m_device->Initialize();

		CreateResources();
	}

	void RendererNew::CreateResources()
	{
		// Create root signature
		CD3DX12_ROOT_SIGNATURE_DESC rsdesc{};
		rsdesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_rootSignature = std::make_unique<RootSignature>(m_device.get(), rsdesc, "Root Signature");

		// Compile shaders
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

		// Create pipeline state
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		
		Shader::Blob* const vsBlob = m_vs.GetBlob();
		Shader::Blob* const psBlob = m_ps.GetBlob();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.InputLayout                     = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature                  = *m_rootSignature;
		psoDesc.VS                              = { reinterpret_cast<byte*>(vsBlob->GetBufferPointer()), vsBlob->GetBufferSize() };
		psoDesc.PS                              = { reinterpret_cast<byte*>(psBlob->GetBufferPointer()), psBlob->GetBufferSize() };
		psoDesc.RasterizerState                 = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState                      = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable   = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask                      = UINT_MAX;
		psoDesc.PrimitiveTopologyType           = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets                = 1;
		psoDesc.RTVFormats[0]                   = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count                = 1;

		m_pipelineState = std::make_unique<PipelineState>(m_device.get(), psoDesc, "Graphics Pipeline State");
	
		// Create vertex buffer
		Buffer::Desc desc
		{
			.SizeInBytes   = Primitives::TriangleVertexBufferSize,
			.StrideInBytes = sizeof(Primitives::TriangleVertexPosCol),
			.Usage         = Buffer::Usage::Default,  // GPU only memory
			.Type          = Buffer::Type::Vertex,
			.Name          = "Triangle Vertex Buffer"
		};

		m_vertexBuffer = std::make_unique<Buffer>(m_device.get(), desc, Primitives::TriangleVerticesPosCol.data());
	}
	
	void RendererNew::Render()
	{
		CommandList* cmdList  = m_device->GetGraphicsCommandList();
		Texture* renderTarget = m_device->GetCurrentBackBuffer();

		m_device->BeginFrame(m_pipelineState.get());

		cmdList->GetNative()->SetGraphicsRootSignature(*m_rootSignature);

		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_device->SetBackBufferRenderTarget(true);

		m_vertexBuffer->FinishUpload(*cmdList);
		cmdList->SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->GetNative()->DrawInstanced(3, 1, 0, 0);

		cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		m_device->EndFrame();
		m_device->Present();
	}
	
	void RendererNew::OnResize(u32 w, u32 h)
	{
		m_device->ResizeBuffers(w, h);
	}
}
