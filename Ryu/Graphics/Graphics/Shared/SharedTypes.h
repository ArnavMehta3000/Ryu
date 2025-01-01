#pragma once
#include "Common/Enum.h"

namespace Ryu::Graphics
{
	enum class Format
	{
		Unknown,

		R32G32B32A32_TYPELESS,
		R32G32B32_TYPELESS,
		R32G32_TYPELESS,
		R32_TYPELESS,

		R16G16B16A16_TYPELESS,
		R16G16_TYPELESS,
		R16_TYPELESS,

		R8G8B8A8_TYPELESS,
		R8G8_TYPELESS,
		R8_TYPELESS,
		B8G8R8A8_TYPELESS,

		R32G32B32A32_FLOAT,
		R32G32B32_FLOAT,
		R32G32_FLOAT,
		R32_FLOAT,

		R16G16B16A16_FLOAT,
		R16G16_FLOAT,
		R16_FLOAT,

		R32G32B32A32_UINT,
		R32G32B32_UINT,
		R32G32_UINT,
		R32_UINT,

		R16G16B16A16_UINT,
		R16G16_UINT,
		R16_UINT,

		R8G8B8A8_UINT,
		R8G8_UINT,
		R8_UINT,

		R32G32B32A32_SINT,
		R32G32B32_SINT,
		R32G32_SINT,
		R32_SINT,

		R16G16B16A16_SINT,
		R16G16_SINT,
		R16_SINT,

		R8G8B8A8_SINT,
		R8G8_SINT,
		R8_SINT,

		R16G16B16A16_UNORM,
		R16G16_UNORM,
		R16_UNORM,

		R8G8B8A8_UNORM,
		R8G8B8A8_UNORM_SRGB,
		R8G8_UNORM,
		R8_UNORM,
		B8G8R8A8_UNORM,
		B8G8R8A8_UNORM_SRGB,
		B8G8R8X8_UNORM,
		B8G8R8X8_UNORM_SRGB,

		R16G16B16A16_SNORM,
		R16G16_SNORM,
		R16_SNORM,

		R8G8B8A8_SNORM,
		R8G8_SNORM,
		R8_SNORM,

		D32_FLOAT,
		D16_UNORM,
		D32_FLOAT_S8_UINT,
		R32_FLOAT_X32_TYPELESS,

		B4G4R4A4_UNORM,
		B5G6R5_UNORM,
		B5G5R5A1_UNORM,

		R9G9B9E5_SHAREDEXP,
		R10G10B10A2_TYPELESS,
		R10G10B10A2_UNORM,
		R10G10B10A2_UINT,
		R11G11B10_FLOAT,

		BC1_UNORM,
		BC1_UNORM_SRGB,
		BC2_UNORM,
		BC2_UNORM_SRGB,
		BC3_UNORM,
		BC3_UNORM_SRGB,
		BC4_UNORM,
		BC4_SNORM,
		BC5_UNORM,
		BC5_SNORM,
		BC6H_UF16,
		BC6H_SF16,
		BC7_UNORM,
		BC7_UNORM_SRGB,

		R64_UINT,
		R64_SINT,

		_Count,
	};

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
