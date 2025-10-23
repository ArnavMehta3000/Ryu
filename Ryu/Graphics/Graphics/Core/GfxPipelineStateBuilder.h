#pragma once
#include "Graphics/Core/DX12.h"
#include <optional>

namespace Ryu::Gfx
{
	class GfxDevice;

	class GfxPipelineStateBuilder
	{
	public:
		explicit GfxPipelineStateBuilder(GfxDevice* device);

		GfxPipelineStateBuilder& SetVertexShader(const void* bytecode, size_t size);
		GfxPipelineStateBuilder& SetPixelShader(const void* bytecode, size_t size);
		GfxPipelineStateBuilder& SetComputeShader(const void* bytecode, size_t size);

		GfxPipelineStateBuilder& SetInputLayout(const D3D12_INPUT_ELEMENT_DESC* elements, u32 count);

		GfxPipelineStateBuilder& SetRootSignature(ID3D12RootSignature* rootSig);

		GfxPipelineStateBuilder& SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
		GfxPipelineStateBuilder& SetCullMode(D3D12_CULL_MODE mode);
		GfxPipelineStateBuilder& SetFillMode(D3D12_FILL_MODE mode);
		GfxPipelineStateBuilder& SetDepthBias(i32 bias);

		GfxPipelineStateBuilder& SetBlendState(const D3D12_BLEND_DESC& desc);
		GfxPipelineStateBuilder& EnableAlphaBlending(u32 rtIndex = 0);

		GfxPipelineStateBuilder& SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);
		GfxPipelineStateBuilder& EnableDepthTest(bool enable = true);
		GfxPipelineStateBuilder& EnableDepthWrite(bool enable = true);
		GfxPipelineStateBuilder& SetDepthFunc(D3D12_COMPARISON_FUNC func);

		GfxPipelineStateBuilder& SetRenderTargetFormat(u32 index, DXGI_FORMAT format);
		GfxPipelineStateBuilder& SetRenderTargetFormats(const DXGI_FORMAT* formats, u32 count);
		GfxPipelineStateBuilder& SetDepthStencilFormat(DXGI_FORMAT format);

		GfxPipelineStateBuilder& SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

		GfxPipelineStateBuilder& SetSampleCount(u32 count, u32 quality = 0);

		ComPtr<ID3D12PipelineState> Build(const char* debugName = nullptr);

		// Helper function to create a simple graphics PSO
		inline static ComPtr<ID3D12PipelineState> CreateSimple(
			GfxDevice* device,
			ID3D12RootSignature* rootSig,
			const void* vsBytecode, size_t vsSize,
			const void* psBytecode, size_t psSize,
			const D3D12_INPUT_ELEMENT_DESC* inputLayout, u32 inputCount,
			DXGI_FORMAT rtvFormat,
			const char* debugName = nullptr)
		{
			return GfxPipelineStateBuilder(device)
				.SetRootSignature(rootSig)
				.SetVertexShader(vsBytecode, vsSize)
				.SetPixelShader(psBytecode, psSize)
				.SetInputLayout(inputLayout, inputCount)
				.SetRenderTargetFormat(0, rtvFormat)
				.SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
				.Build(debugName);
		}

	private:
		GfxDevice*                            m_device = nullptr;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC    m_desc{};
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElements;
		bool                                  m_isCompute = false;
	};
}
