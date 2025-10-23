#include "Graphics/Core/GfxPipelineStateBuilder.h"
#include "Graphics/Core/GfxDevice.h"
#include "Common/Assert.h"

namespace Ryu::Gfx
{
	GfxPipelineStateBuilder::GfxPipelineStateBuilder(GfxDevice* device)
		: m_device(device)
	{
		RYU_ASSERT(m_device, "GfxDevice cannot be null");

		// Set defaults
		m_desc.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		m_desc.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		m_desc.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		m_desc.SampleMask            = UINT_MAX;
		m_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_desc.NumRenderTargets      = 0;
		m_desc.SampleDesc.Count      = 1;
		m_desc.SampleDesc.Quality    = 0;
		m_desc.DSVFormat             = DXGI_FORMAT_UNKNOWN;

		for (u32 i = 0; i < 8; i++)
		{
			m_desc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
		}
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetVertexShader(const void* bytecode, size_t size)
	{
		m_desc.VS = { reinterpret_cast<const byte*>(bytecode), size };
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetPixelShader(const void* bytecode, size_t size)
	{
		m_desc.PS = { reinterpret_cast<const byte*>(bytecode), size };
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetComputeShader(const void* bytecode, size_t size)
	{
		m_isCompute = true;
		// Note: Compute PSO uses a different desc structure
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetInputLayout(
		const D3D12_INPUT_ELEMENT_DESC* elements,
		u32 count)
	{
		m_inputElements.assign(elements, elements + count);
		m_desc.InputLayout = { m_inputElements.data(), count };
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetRootSignature(ID3D12RootSignature* rootSig)
	{
		m_desc.pRootSignature = rootSig;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		m_desc.RasterizerState = desc;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetCullMode(D3D12_CULL_MODE mode)
	{
		m_desc.RasterizerState.CullMode = mode;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetFillMode(D3D12_FILL_MODE mode)
	{
		m_desc.RasterizerState.FillMode = mode;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetDepthBias(i32 bias)
	{
		m_desc.RasterizerState.DepthBias = bias;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		m_desc.BlendState = desc;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::EnableAlphaBlending(u32 rtIndex)
	{
		if (rtIndex < 8)
		{
			auto& rt                 = m_desc.BlendState.RenderTarget[rtIndex];
			rt.BlendEnable           = TRUE;
			rt.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
			rt.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
			rt.BlendOp               = D3D12_BLEND_OP_ADD;
			rt.SrcBlendAlpha         = D3D12_BLEND_ONE;
			rt.DestBlendAlpha        = D3D12_BLEND_ZERO;
			rt.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
			rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		m_desc.DepthStencilState = desc;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::EnableDepthTest(bool enable)
	{
		m_desc.DepthStencilState.DepthEnable = enable ? TRUE : FALSE;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::EnableDepthWrite(bool enable)
	{
		m_desc.DepthStencilState.DepthWriteMask = enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetDepthFunc(D3D12_COMPARISON_FUNC func)
	{
		m_desc.DepthStencilState.DepthFunc = func;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetRenderTargetFormat(u32 index, DXGI_FORMAT format)
	{
		if (index < 8)
		{
			m_desc.RTVFormats[index] = format;
			m_desc.NumRenderTargets = std::max(m_desc.NumRenderTargets, index + 1);
		}
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetRenderTargetFormats(const DXGI_FORMAT* formats, u32 count)
	{
		count = std::min(count, 8u);
		for (u32 i = 0; i < count; i++)
		{
			m_desc.RTVFormats[i] = formats[i];
		}
		m_desc.NumRenderTargets = count;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetDepthStencilFormat(DXGI_FORMAT format)
	{
		m_desc.DSVFormat = format;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		m_desc.PrimitiveTopologyType = type;
		return *this;
	}

	GfxPipelineStateBuilder& GfxPipelineStateBuilder::SetSampleCount(u32 count, u32 quality)
	{
		m_desc.SampleDesc.Count = count;
		m_desc.SampleDesc.Quality = quality;
		return *this;
	}

	ComPtr<ID3D12PipelineState> GfxPipelineStateBuilder::Build(const char* debugName)
	{
		RYU_ASSERT(m_desc.pRootSignature, "Root signature must be set");
		RYU_ASSERT(!m_isCompute, "Compute PSO not yet implemented");

		ComPtr<ID3D12PipelineState> pso;
		DXCall(m_device->GetDevice()->CreateGraphicsPipelineState(&m_desc, IID_PPV_ARGS(&pso)));

		if (debugName)
		{
			DX12::SetObjectName(pso.Get(), debugName);
		}

		return pso;
	}
}
