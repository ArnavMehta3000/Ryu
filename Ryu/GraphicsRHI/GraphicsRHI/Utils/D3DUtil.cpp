#include "D3DUtil.h"
#include "GraphicsRHI/Utils/Logging.h"

namespace Ryu::Graphics::Utils
{
	DXGI_FORMAT GetFormat(Format format)
	{
		return static_cast<DXGI_FORMAT>(format);
	}
	
	Format GetFormat(DXGI_FORMAT format)
	{
		return static_cast<Format>(format);
	}

	DXGI_FORMAT GetIndexFormat(IndexFormat format)
	{
		switch (format)
		{
		case IndexFormat::UInt16: return DXGI_FORMAT_R16_UINT;
		case IndexFormat::UInt32: return DXGI_FORMAT_R32_UINT;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown index format {}", static_cast<UINT>(format));
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT ToNonSRGBFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
		default:                                return format;
		}
	}

	DXGI_SWAP_EFFECT GetSwapEffect(SwapEffect effect)
	{
		switch (effect)
		{
		case SwapEffect::Discard:        return DXGI_SWAP_EFFECT_DISCARD;
		case SwapEffect::Sequential:     return DXGI_SWAP_EFFECT_SEQUENTIAL;
		case SwapEffect::FlipDiscard:    return DXGI_SWAP_EFFECT_FLIP_DISCARD;
		case SwapEffect::FlipSequential: return DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown swap effect {}. Defaulting to discard", static_cast<UINT>(effect));
			return DXGI_SWAP_EFFECT_DISCARD;
		}
	}

	bool IsFormatTypeless(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC7_TYPELESS:
		{
			return true;
		}
		}

		return false;
	}

	D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology(PrimitiveTopology topology)
	{
		switch (topology)
		{
		case PrimitiveTopology::PointList:     return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case PrimitiveTopology::LineList:      return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case PrimitiveTopology::LineStrip:     return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PrimitiveTopology::TriangleList:  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case PrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown primitive topology {}. Defaulting to undefined", static_cast<UINT>(topology));
			return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}

	D3D11_STENCIL_OP GetStencilOp(StencilOp op)
	{
		switch (op)
		{
		case StencilOp::Keep:              return D3D11_STENCIL_OP_KEEP;
		case StencilOp::Zero:              return D3D11_STENCIL_OP_ZERO;
		case StencilOp::Replace:           return D3D11_STENCIL_OP_REPLACE;
		case StencilOp::IncrementSaturate: return D3D11_STENCIL_OP_INCR_SAT;
		case StencilOp::DecrementSaturate: return D3D11_STENCIL_OP_DECR_SAT;
		case StencilOp::Invert:            return D3D11_STENCIL_OP_INVERT;
		case StencilOp::IncrementWrap:     return D3D11_STENCIL_OP_INCR;
		case StencilOp::DecrementWrap:     return D3D11_STENCIL_OP_DECR;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown stencil op. Defaulting to keep");
			return D3D11_STENCIL_OP_KEEP;
		}
	}

	D3D11_COMPARISON_FUNC GetComparisonFunc(ComparisonFunc func)
	{
		switch (func)
		{
		case ComparisonFunc::Never:        return D3D11_COMPARISON_NEVER;
		case ComparisonFunc::Less:         return D3D11_COMPARISON_LESS;
		case ComparisonFunc::Equal:        return D3D11_COMPARISON_EQUAL;
		case ComparisonFunc::LessEqual:    return D3D11_COMPARISON_LESS_EQUAL;
		case ComparisonFunc::Greater:      return D3D11_COMPARISON_GREATER;
		case ComparisonFunc::NotEqual:     return D3D11_COMPARISON_NOT_EQUAL;
		case ComparisonFunc::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
		case ComparisonFunc::Always:       return D3D11_COMPARISON_ALWAYS;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown comparison func. Defaulting to never");
			return D3D11_COMPARISON_NEVER;
		}
	}

	D3D11_FILL_MODE GetFillMode(FillMode mode)
	{
		switch (mode)
		{
		case FillMode::Solid:     return D3D11_FILL_SOLID;
		case FillMode::Wireframe: return D3D11_FILL_WIREFRAME;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown fill mode. Defaulting to solid");
			return D3D11_FILL_SOLID;
		}
	}

	D3D11_CULL_MODE GetCullMode(CullMode mode)
	{
		switch (mode)
		{
		case CullMode::None:  return D3D11_CULL_NONE;
		case CullMode::Front: return D3D11_CULL_FRONT;
		case CullMode::Back:  return D3D11_CULL_BACK;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown cull mode. Defaulting to none");
			return D3D11_CULL_NONE;
		}
	}

	D3D11_BLEND_OP GetBlendOp(BlendOp op)
	{
		switch (op)
		{
		case BlendOp::Add:             return D3D11_BLEND_OP_ADD;
		case BlendOp::Subtract:        return D3D11_BLEND_OP_SUBTRACT;
		case BlendOp::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Min:             return D3D11_BLEND_OP_MIN;
		case BlendOp::Max:             return D3D11_BLEND_OP_MAX;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown blend op. Defaulting to add");
			return D3D11_BLEND_OP_ADD;
		}
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(PrimitiveTopology topology)
	{
		switch (topology)
		{
		case PrimitiveTopology::PointList: 
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		
		case PrimitiveTopology::LineList:
		case PrimitiveTopology::LineStrip:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		
		case PrimitiveTopology::TriangleList:
		case PrimitiveTopology::TriangleStrip:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		
		case PrimitiveTopology::PatchList:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown primitive topology. Defaulting to point list");
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}
	}
	
	D3D12_STENCIL_OP GetStencilOp12(StencilOp op)
	{
		switch (op)
		{
		case StencilOp::Keep:              return D3D12_STENCIL_OP_KEEP;
		case StencilOp::Zero:              return D3D12_STENCIL_OP_ZERO;
		case StencilOp::Replace:           return D3D12_STENCIL_OP_REPLACE;
		case StencilOp::IncrementSaturate: return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOp::DecrementSaturate: return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOp::Invert:            return D3D12_STENCIL_OP_INVERT;
		case StencilOp::IncrementWrap:     return D3D12_STENCIL_OP_INCR;
		case StencilOp::DecrementWrap:     return D3D12_STENCIL_OP_DECR;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown stencil op. Defaulting to keep");
			return D3D12_STENCIL_OP_KEEP;
		}
	}
	
	D3D12_COMPARISON_FUNC GetComparisonFunc12(ComparisonFunc func)
	{
		switch (func)
		{
		case ComparisonFunc::Never:        return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::Less:         return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::Equal:        return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::LessEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::Greater:      return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::NotEqual:     return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::Always:       return D3D12_COMPARISON_FUNC_ALWAYS;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown comparison func. Defaulting to never");
			return D3D12_COMPARISON_FUNC_NEVER;
		}
	}
	
	D3D12_FILL_MODE GetFillMode12(FillMode mode)
	{
		switch (mode)
		{
		case FillMode::Solid:     return D3D12_FILL_MODE_SOLID;
		case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown fill mode. Defaulting to solid");
			return D3D12_FILL_MODE_SOLID;
		}
	}
	
	D3D12_CULL_MODE GetCullMode12(CullMode mode)
	{
		switch (mode)
		{
		case CullMode::None:  return D3D12_CULL_MODE_NONE;
		case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		case CullMode::Back:  return D3D12_CULL_MODE_BACK;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown cull mode. Defaulting to none");
			return D3D12_CULL_MODE_NONE;
		}
	}
	
	D3D12_BLEND_OP GetBlendOp12(BlendOp op)
	{
		switch (op)
		{
		case BlendOp::Add:             return D3D12_BLEND_OP_ADD;
		case BlendOp::Subtract:        return D3D12_BLEND_OP_SUBTRACT;
		case BlendOp::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Min:             return D3D12_BLEND_OP_MIN;
		case BlendOp::Max:             return D3D12_BLEND_OP_MAX;
		default:
			RYU_LOG_WARN(Internal::GraphicsPanicLog, "Unknown blend op. Defaulting to add");
			return D3D12_BLEND_OP_ADD;
		}
	}
}

