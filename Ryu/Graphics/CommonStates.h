#pragma once
#include <nvrhi/thirdparty/DirectX-Headers/include/directx/d3dx12.h>

namespace Ryu::Gfx::CommonStates
{
   // ---------- Blend States ----------

	[[nodiscard]] inline static CD3DX12_BLEND_DESC BSOpaque()
	{
		static CD3DX12_BLEND_DESC desc(CD3DX12_DEFAULT{});
		return desc;
	}

	[[nodiscard]] inline static CD3DX12_BLEND_DESC BSAplhaBlend()
	{
		static CD3DX12_BLEND_DESC desc(CD3DX12_DEFAULT{});
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;

		return desc;
	}

	[[nodiscard]] inline static CD3DX12_BLEND_DESC BSAdditive()
	{
		static CD3DX12_BLEND_DESC desc(CD3DX12_DEFAULT{});
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;

		return desc;
	}

	[[nodiscard]] inline static CD3DX12_BLEND_DESC BSNonPremultiplied()
	{
		CD3DX12_BLEND_DESC desc(CD3DX12_DEFAULT{});
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend    = desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend   = desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;

		return desc;
	}

	[[nodiscard]] inline static CD3DX12_DEPTH_STENCIL_DESC DSNone()
	{
		static CD3DX12_DEPTH_STENCIL_DESC desc(CD3DX12_DEFAULT{});
		desc.DepthEnable    = FALSE;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		return desc;
	}
	
	// ---------- Depth Stencil States ----------

	[[nodiscard]] inline static CD3DX12_DEPTH_STENCIL_DESC DSDefault()
	{
		static CD3DX12_DEPTH_STENCIL_DESC desc(CD3DX12_DEFAULT{});
		desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		return desc;
	}
	
	[[nodiscard]] inline static CD3DX12_DEPTH_STENCIL_DESC DSRead()
	{
		static CD3DX12_DEPTH_STENCIL_DESC desc(CD3DX12_DEFAULT{});
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		return desc;
	}
	
	[[nodiscard]] inline static CD3DX12_DEPTH_STENCIL_DESC DSReverseZ()
	{
		static CD3DX12_DEPTH_STENCIL_DESC desc(CD3DX12_DEFAULT{});
		desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;

		return desc;
	}

	[[nodiscard]] inline static CD3DX12_DEPTH_STENCIL_DESC DSReadReverseZ()
	{
		static CD3DX12_DEPTH_STENCIL_DESC desc(CD3DX12_DEFAULT{});
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;

		return desc;
	}

	// ---------- Rasterizer States ----------

	[[nodiscard]] inline static CD3DX12_RASTERIZER_DESC RSCullNone()
	{
		static CD3DX12_RASTERIZER_DESC desc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE,
			FALSE,  // FrontCounterClockwise
			D3D12_DEFAULT_DEPTH_BIAS,
			D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE,   // DepthClipEnable
			TRUE,   // MultisampleEnable
			FALSE,  // AntialiasedLineEnable
			0,      // ForceSampleCount
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

		return desc;
	}

	[[nodiscard]] inline static CD3DX12_RASTERIZER_DESC RSCullClockwise()
	{
		static CD3DX12_RASTERIZER_DESC desc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_FRONT,
			FALSE,  // FrontCounterClockwise
			D3D12_DEFAULT_DEPTH_BIAS,
			D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE,   // DepthClipEnable
			TRUE,   // MultisampleEnable
			FALSE,  // AntialiasedLineEnable
			0,      // ForceSampleCount
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

		return desc;
	}

	[[nodiscard]] inline static CD3DX12_RASTERIZER_DESC RSCullCounterClockwise()
	{
		static CD3DX12_RASTERIZER_DESC desc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK,
			FALSE,  // FrontCounterClockwise
			D3D12_DEFAULT_DEPTH_BIAS,
			D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE,   // DepthClipEnable
			TRUE,   // MultisampleEnable
			FALSE,  // AntialiasedLineEnable
			0,      // ForceSampleCount
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

		return desc;
	}

	[[nodiscard]] inline static CD3DX12_RASTERIZER_DESC RSWireframe()
	{
		static CD3DX12_RASTERIZER_DESC desc(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE,
			FALSE,  // FrontCounterClockwise
			D3D12_DEFAULT_DEPTH_BIAS,
			D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE,   // DepthClipEnable
			TRUE,   // MultisampleEnable
			FALSE,  // AntialiasedLineEnable
			0,      // ForceSampleCount
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

		return desc;
	}

	// ---------- Sampler States ----------

	[[nodiscard]] inline static D3D12_SAMPLER_DESC SSPointWrap()
	{
	   static D3D12_SAMPLER_DESC desc
	   { 
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0, D3D12_MAX_MAXANISOTROPY, D3D12_COMPARISON_FUNC_NEVER,
			{ 0, 0, 0, 0 }, 0, D3D12_FLOAT32_MAX
	   };

	   return desc;
	}

	[[nodiscard]] inline static D3D12_SAMPLER_DESC SSPointClamp()
	{
		static D3D12_SAMPLER_DESC desc
		{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0, D3D12_MAX_MAXANISOTROPY, D3D12_COMPARISON_FUNC_NEVER,
			{ 0, 0, 0, 0 }, 0, D3D12_FLOAT32_MAX
		};

		return desc;
	}

	[[nodiscard]] inline static D3D12_SAMPLER_DESC SSLinearWrap()
	{
		static D3D12_SAMPLER_DESC desc
		{
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0, D3D12_MAX_MAXANISOTROPY, D3D12_COMPARISON_FUNC_NEVER,
			{ 0, 0, 0, 0 }, 0, D3D12_FLOAT32_MAX
		};

		return desc;
	}

	[[nodiscard]] inline static D3D12_SAMPLER_DESC SSLinearClamp()
	{
		static D3D12_SAMPLER_DESC desc
		{
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0, D3D12_MAX_MAXANISOTROPY, D3D12_COMPARISON_FUNC_NEVER,
			{ 0, 0, 0, 0 }, 0, D3D12_FLOAT32_MAX
		};

		return desc;
	}

	[[nodiscard]] inline static D3D12_SAMPLER_DESC SSAnisotropicWrap()
	{
		static D3D12_SAMPLER_DESC desc
		{
			D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0, D3D12_MAX_MAXANISOTROPY, D3D12_COMPARISON_FUNC_NEVER,
			{ 0, 0, 0, 0 }, 0, D3D12_FLOAT32_MAX
		};

		return desc;
	}

	[[nodiscard]] inline static D3D12_SAMPLER_DESC SSAnisotropicClamp()
	{
		static D3D12_SAMPLER_DESC desc
		{
			D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0, D3D12_MAX_MAXANISOTROPY, D3D12_COMPARISON_FUNC_NEVER,
			{ 0, 0, 0, 0 }, 0, D3D12_FLOAT32_MAX
		};

		return desc;
	}
}
