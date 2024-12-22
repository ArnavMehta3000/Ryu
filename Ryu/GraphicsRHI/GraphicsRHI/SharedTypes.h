#pragma once
#include "Common/Enum.h"
#include <dxgiformat.h>

namespace Ryu::Graphics
{
	enum class Format : int
	{
#define DECLARE_FORMAT(name) name = DXGI_FORMAT_##name
		Unknown = DXGI_FORMAT_UNKNOWN,
		
		DECLARE_FORMAT(R32G32B32A32_TYPELESS),
		DECLARE_FORMAT(R32G32B32A32_FLOAT),
		DECLARE_FORMAT(R32G32B32A32_UINT),
		DECLARE_FORMAT(R32G32B32A32_SINT),

		DECLARE_FORMAT(R32G32B32_TYPELESS),
		DECLARE_FORMAT(R32G32B32_FLOAT),
		DECLARE_FORMAT(R32G32B32_UINT),
		DECLARE_FORMAT(R32G32B32_SINT),

		DECLARE_FORMAT(R16G16B16A16_TYPELESS),
		DECLARE_FORMAT(R16G16B16A16_FLOAT),
		DECLARE_FORMAT(R16G16B16A16_UINT),
		DECLARE_FORMAT(R16G16B16A16_SINT),

		DECLARE_FORMAT(R32G32_TYPELESS),
		DECLARE_FORMAT(R32G32_FLOAT),
		DECLARE_FORMAT(R32G32_UINT),
		DECLARE_FORMAT(R32G32_SINT),

		DECLARE_FORMAT(R32G8X24_TYPELESS),
		DECLARE_FORMAT(D32_FLOAT_S8X24_UINT),
		DECLARE_FORMAT(R32_FLOAT_X8X24_TYPELESS),
		DECLARE_FORMAT(X32_TYPELESS_G8X24_UINT),

		DECLARE_FORMAT(R10G10B10A2_TYPELESS),
		DECLARE_FORMAT(R10G10B10A2_UINT),

		DECLARE_FORMAT(R11G11B10_FLOAT),

		DECLARE_FORMAT(R8G8B8A8_TYPELESS),
		DECLARE_FORMAT(R8G8B8A8_UNORM),
		DECLARE_FORMAT(R8G8B8A8_UNORM_SRGB),
		DECLARE_FORMAT(R8G8B8A8_UINT),
		DECLARE_FORMAT(R8G8B8A8_SNORM),
		DECLARE_FORMAT(R8G8B8A8_SINT),

		DECLARE_FORMAT(R16G16_TYPELESS),
		DECLARE_FORMAT(R16G16_FLOAT),
		DECLARE_FORMAT(R16G16_UINT),
		DECLARE_FORMAT(R16G16_SNORM),
		DECLARE_FORMAT(R16G16_SINT),

		DECLARE_FORMAT(R32_TYPELESS),
		DECLARE_FORMAT(D32_FLOAT),
		DECLARE_FORMAT(R32_FLOAT),
		DECLARE_FORMAT(R32_UINT),
		DECLARE_FORMAT(R32_SINT),

		DECLARE_FORMAT(R24G8_TYPELESS),
		DECLARE_FORMAT(D24_UNORM_S8_UINT),
		DECLARE_FORMAT(R24_UNORM_X8_TYPELESS),
		DECLARE_FORMAT(X24_TYPELESS_G8_UINT),

		DECLARE_FORMAT(R8G8_TYPELESS),
		DECLARE_FORMAT(R8G8_UNORM),
		DECLARE_FORMAT(R8G8_UINT),
		DECLARE_FORMAT(R8G8_SNORM),
		DECLARE_FORMAT(R8G8_SINT),

		DECLARE_FORMAT(R16_TYPELESS),
		DECLARE_FORMAT(R16_FLOAT),
		DECLARE_FORMAT(D16_UNORM),
		DECLARE_FORMAT(R16_UNORM),
		DECLARE_FORMAT(R16_UINT),
		DECLARE_FORMAT(R16_SNORM),
		DECLARE_FORMAT(R16_SINT),

		DECLARE_FORMAT(R8_TYPELESS),
		DECLARE_FORMAT(R8_UNORM),
		DECLARE_FORMAT(R8_UINT),
		DECLARE_FORMAT(R8_SNORM),
		DECLARE_FORMAT(R8_SINT),
		DECLARE_FORMAT(A8_UNORM),
		DECLARE_FORMAT(R1_UNORM),

		DECLARE_FORMAT(R9G9B9E5_SHAREDEXP),
		DECLARE_FORMAT(R8G8_B8G8_UNORM),
		DECLARE_FORMAT(G8R8_G8B8_UNORM),

		DECLARE_FORMAT(BC1_TYPELESS),
		DECLARE_FORMAT(BC1_UNORM),
		DECLARE_FORMAT(BC1_UNORM_SRGB),
		DECLARE_FORMAT(BC2_TYPELESS),
		DECLARE_FORMAT(BC2_UNORM),
		DECLARE_FORMAT(BC2_UNORM_SRGB),
		DECLARE_FORMAT(BC3_TYPELESS),
		DECLARE_FORMAT(BC3_UNORM),
		DECLARE_FORMAT(BC3_UNORM_SRGB),
		DECLARE_FORMAT(BC4_TYPELESS),
		DECLARE_FORMAT(BC4_UNORM),
		DECLARE_FORMAT(BC4_SNORM),
		DECLARE_FORMAT(BC5_TYPELESS),
		DECLARE_FORMAT(BC5_UNORM),
		DECLARE_FORMAT(BC5_SNORM),

		DECLARE_FORMAT(B5G6R5_UNORM),
		DECLARE_FORMAT(B5G5R5A1_UNORM),
		DECLARE_FORMAT(B8G8R8A8_UNORM),
		DECLARE_FORMAT(B8G8R8X8_UNORM),

		DECLARE_FORMAT(R10G10B10_XR_BIAS_A2_UNORM),

		DECLARE_FORMAT(B8G8R8A8_TYPELESS),
		DECLARE_FORMAT(B8G8R8A8_UNORM_SRGB),
		DECLARE_FORMAT(B8G8R8X8_TYPELESS),
		DECLARE_FORMAT(B8G8R8X8_UNORM_SRGB),

		DECLARE_FORMAT(BC6H_TYPELESS),
		DECLARE_FORMAT(BC6H_UF16),    
		DECLARE_FORMAT(BC6H_SF16),
		
		DECLARE_FORMAT(BC7_TYPELESS),
		DECLARE_FORMAT(BC7_UNORM),
		DECLARE_FORMAT(BC7_UNORM_SRGB),

		DECLARE_FORMAT(AYUV),
		DECLARE_FORMAT(Y410),
		DECLARE_FORMAT(Y416),
		DECLARE_FORMAT(NV12),
		DECLARE_FORMAT(P010),
		DECLARE_FORMAT(P016),
		_420_OPAQUE = DXGI_FORMAT_420_OPAQUE,
		DECLARE_FORMAT(YUY2),
		DECLARE_FORMAT(Y210),
		DECLARE_FORMAT(Y216),
		DECLARE_FORMAT(NV11),
		DECLARE_FORMAT(AI44),
		DECLARE_FORMAT(IA44),
		DECLARE_FORMAT(P8),
		DECLARE_FORMAT(A8P8),
		
		DECLARE_FORMAT(B4G4R4A4_UNORM),

		DECLARE_FORMAT(P208),
		DECLARE_FORMAT(V208),
		DECLARE_FORMAT(V408),

		DECLARE_FORMAT(SAMPLER_FEEDBACK_MIN_MIP_OPAQUE),
		DECLARE_FORMAT(SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE),
		
		DECLARE_FORMAT(FORCE_UINT),
#undef DECLARE_FORMAT
	};

	enum class TextureUsage
	{
		None            = 0,
		RenderTarget    = RYU_BIT(0),
		DepthStencil    = RYU_BIT(1),
		ShaderResource  = RYU_BIT(2),
		UnorderedAccess = RYU_BIT(3)
	};
	RYU_ENABLE_BITMASK_OPERATORS(TextureUsage)

	enum class SwapEffect
	{
		Discard,
		Sequential,
		FlipDiscard,
		FlipSequential,
	};

	enum class SwapChainFlag
	{
		AllowModeSwitch = RYU_BIT(0),
		AllowFullscreen = RYU_BIT(1),
		AllowTearing    = RYU_BIT(2),
	};
	RYU_ENABLE_BITMASK_OPERATORS(SwapChainFlag)

	enum class IndexFormat
	{
		UInt16,
		UInt32,
	};

	enum class StencilOp
	{
		Keep,
		Zero,
		Replace,
		IncrementSaturate,
		DecrementSaturate,
		Invert,
		IncrementWrap,
		DecrementWrap,
	};

	enum class ComparisonFunc
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,
	};

	enum class FillMode
	{
		Solid,
		Wireframe
	};

	enum class CullMode
	{
		None,
		Front,
		Back
	};

	enum class FrontFaceMode
	{
		CounterClockwise,
		Clockwise
	};

	enum class PrimitiveTopology
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		PatchList,
	};

	enum class CpuAccessMode
	{
		Read,
		Write,
	};

	enum class TextureFilteringMode
	{
		Point,
		Linear,
	};

	enum class TextureAddressingMode
	{
		Wrap,
		ClampToEdge,
		ClampToBorder,
		MirrorRepeat,
		MirrorOnce,
	};

	enum class TextureReductionOp
	{
		Average,
		Comparison,
		Minimum,
		Maximum,
	};

	enum class BlendOp
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max,
	};

	enum class BlendFactor
	{
		Zero,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,
		DestColor,
		InvDestColor,
		SrcAlphaSaturate,
		BlendColor,
		InvBlendColor,
		SecondarySrcColor,
		InvSecondarySrcColor,
		SecondarySrcAlpha,
		InvSecondarySrcAlpha,
	};
}
