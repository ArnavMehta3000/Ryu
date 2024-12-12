#include "D3DUtil.h"
#include <d3d12.h>
#include <d3d11.h>
#include <dxgidebug.h>
#include "Graphics/Shared/Logging.h"

namespace Ryu::Graphics::Utils
{
	DXGI_FORMAT GetFormat(Format format)
	{
		switch (format)
		{
		case Format::R32G32B32A32_TYPELESS:  return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		case Format::R32G32B32_TYPELESS:     return DXGI_FORMAT_R32G32B32_TYPELESS;
		case Format::R32G32_TYPELESS:        return DXGI_FORMAT_R32G32_TYPELESS;
		case Format::R32_TYPELESS:           return DXGI_FORMAT_R32_TYPELESS;

		case Format::R16G16B16A16_TYPELESS:  return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case Format::R16G16_TYPELESS:        return DXGI_FORMAT_R16G16_TYPELESS;
		case Format::R16_TYPELESS:           return DXGI_FORMAT_R16_TYPELESS;

		case Format::R8G8B8A8_TYPELESS:      return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case Format::R8G8_TYPELESS:          return DXGI_FORMAT_R8G8_TYPELESS;
		case Format::R8_TYPELESS:            return DXGI_FORMAT_R8_TYPELESS;
		case Format::B8G8R8A8_TYPELESS:      return DXGI_FORMAT_B8G8R8A8_TYPELESS;

		case Format::R32G32B32A32_FLOAT:     return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Format::R32G32B32_FLOAT:        return DXGI_FORMAT_R32G32B32_FLOAT;
		case Format::R32G32_FLOAT:           return DXGI_FORMAT_R32G32_FLOAT;
		case Format::R32_FLOAT:              return DXGI_FORMAT_R32_FLOAT;

		case Format::R16G16B16A16_FLOAT:     return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case Format::R16G16_FLOAT:           return DXGI_FORMAT_R16G16_FLOAT;
		case Format::R16_FLOAT:              return DXGI_FORMAT_R16_FLOAT;

		case Format::R64_UINT:               return DXGI_FORMAT_R32G32_UINT;

		case Format::R32G32B32A32_UINT:      return DXGI_FORMAT_R32G32B32A32_UINT;
		case Format::R32G32B32_UINT:         return DXGI_FORMAT_R32G32B32_UINT;
		case Format::R32G32_UINT:            return DXGI_FORMAT_R32G32_UINT;
		case Format::R32_UINT:               return DXGI_FORMAT_R32_UINT;

		case Format::R16G16B16A16_UINT:      return DXGI_FORMAT_R16G16B16A16_UINT;
		case Format::R16G16_UINT:            return DXGI_FORMAT_R16G16_UINT;
		case Format::R16_UINT:               return DXGI_FORMAT_R16_UINT;

		case Format::R8G8B8A8_UINT:          return DXGI_FORMAT_R8G8B8A8_UINT;
		case Format::R8G8_UINT:              return DXGI_FORMAT_R8G8_UINT;
		case Format::R8_UINT:                return DXGI_FORMAT_R8_UINT;

		case Format::R64_SINT:               return DXGI_FORMAT_R32G32_SINT;

		case Format::R32G32B32A32_SINT:      return DXGI_FORMAT_R32G32B32A32_SINT;
		case Format::R32G32B32_SINT:         return DXGI_FORMAT_R32G32B32_SINT;
		case Format::R32G32_SINT:            return DXGI_FORMAT_R32G32_SINT;
		case Format::R32_SINT:               return DXGI_FORMAT_R32_SINT;

		case Format::R16G16B16A16_SINT:      return DXGI_FORMAT_R16G16B16A16_SINT;
		case Format::R16G16_SINT:            return DXGI_FORMAT_R16G16_SINT;
		case Format::R16_SINT:               return DXGI_FORMAT_R16_SINT;

		case Format::R8G8B8A8_SINT:          return DXGI_FORMAT_R8G8B8A8_SINT;
		case Format::R8G8_SINT:              return DXGI_FORMAT_R8G8_SINT;
		case Format::R8_SINT:                return DXGI_FORMAT_R8_SINT;

		case Format::R16G16B16A16_UNORM:     return DXGI_FORMAT_R16G16B16A16_UNORM;
		case Format::R16G16_UNORM:           return DXGI_FORMAT_R16G16_UNORM;
		case Format::R16_UNORM:              return DXGI_FORMAT_R16_UNORM;

		case Format::R8G8B8A8_UNORM:         return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::R8G8B8A8_UNORM_SRGB:    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case Format::R8G8_UNORM:             return DXGI_FORMAT_R8G8_UNORM;
		case Format::R8_UNORM:               return DXGI_FORMAT_R8_UNORM;
		case Format::B8G8R8A8_UNORM:         return DXGI_FORMAT_B8G8R8A8_UNORM;
		case Format::B8G8R8X8_UNORM:         return DXGI_FORMAT_B8G8R8X8_UNORM;
		case Format::B8G8R8A8_UNORM_SRGB:    return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case Format::B8G8R8X8_UNORM_SRGB:    return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		case Format::R16G16B16A16_SNORM:     return DXGI_FORMAT_R16G16B16A16_SNORM;
		case Format::R16G16_SNORM:           return DXGI_FORMAT_R16G16_SNORM;
		case Format::R16_SNORM:              return DXGI_FORMAT_R16_SNORM;

		case Format::R8G8B8A8_SNORM:         return DXGI_FORMAT_R8G8B8A8_SNORM;
		case Format::R8G8_SNORM:             return DXGI_FORMAT_R8G8_SNORM;
		case Format::R8_SNORM:               return DXGI_FORMAT_R8_SNORM;

		case Format::D32_FLOAT:              return DXGI_FORMAT_D32_FLOAT;
		case Format::D16_UNORM:              return DXGI_FORMAT_D16_UNORM;
		case Format::D32_FLOAT_S8_UINT:      return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case Format::R32_FLOAT_X32_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

		case Format::B4G4R4A4_UNORM:         return DXGI_FORMAT_B4G4R4A4_UNORM;
		case Format::B5G6R5_UNORM:           return DXGI_FORMAT_B5G6R5_UNORM;
		case Format::B5G5R5A1_UNORM:         return DXGI_FORMAT_B5G5R5A1_UNORM;

		case Format::R9G9B9E5_SHAREDEXP:     return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		case Format::R10G10B10A2_TYPELESS:   return DXGI_FORMAT_R10G10B10A2_TYPELESS;
		case Format::R10G10B10A2_UINT:       return DXGI_FORMAT_R10G10B10A2_UINT;
		case Format::R10G10B10A2_UNORM:      return DXGI_FORMAT_R10G10B10A2_UNORM;
		case Format::R11G11B10_FLOAT:        return DXGI_FORMAT_R11G11B10_FLOAT;

		case Format::BC1_UNORM:              return DXGI_FORMAT_BC1_UNORM;
		case Format::BC1_UNORM_SRGB:         return DXGI_FORMAT_BC1_UNORM_SRGB;
		case Format::BC2_UNORM:              return DXGI_FORMAT_BC2_UNORM;
		case Format::BC2_UNORM_SRGB:         return DXGI_FORMAT_BC2_UNORM_SRGB;
		case Format::BC3_UNORM:              return DXGI_FORMAT_BC3_UNORM;
		case Format::BC3_UNORM_SRGB:         return DXGI_FORMAT_BC3_UNORM_SRGB;
		case Format::BC4_UNORM:              return DXGI_FORMAT_BC4_UNORM;
		case Format::BC4_SNORM:              return DXGI_FORMAT_BC4_SNORM;
		case Format::BC5_UNORM:              return DXGI_FORMAT_BC5_UNORM;
		case Format::BC5_SNORM:              return DXGI_FORMAT_BC5_SNORM;
		case Format::BC6H_UF16:              return DXGI_FORMAT_BC6H_UF16;
		case Format::BC6H_SF16:              return DXGI_FORMAT_BC6H_SF16;
		case Format::BC7_UNORM:              return DXGI_FORMAT_BC7_UNORM;
		case Format::BC7_UNORM_SRGB:         return DXGI_FORMAT_BC7_UNORM_SRGB;

		default:                             return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT GetIndexFormat(IndexFormat format)
	{
		switch (format)
		{
		case IndexFormat::UInt16: return DXGI_FORMAT_R16_UINT;
		case IndexFormat::UInt32: return DXGI_FORMAT_R32_UINT;
		default:
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown index format");
			return DXGI_FORMAT_UNKNOWN;
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown primitive topology. Defaulting to undefined");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown stencil op. Defaulting to keep");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown comparison func. Defaulting to never");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown fill mode. Defaulting to solid");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown cull mode. Defaulting to none");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown blend op. Defaulting to add");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown primitive topology. Defaulting to point list");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown stencil op. Defaulting to keep");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown comparison func. Defaulting to never");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown fill mode. Defaulting to solid");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown cull mode. Defaulting to none");
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
			LOG_WARN(Internal::GraphicsPanicLog, "Unknown blend op. Defaulting to add");
			return D3D12_BLEND_OP_ADD;
		}
	}
}

