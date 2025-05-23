#pragma once
#include "Common/StandardTypes.h"
#include "Common/Enum.h"

namespace Ryu::Gfx
{
	class Device;
	class SwapChain;
	class CmdQueue;

	constexpr u32 FRAME_BUFFER_COUNT = 3;

	enum class Format : u8
	{
		Unknown,

		R8_UINT,
		R8_SINT,
		R8_UNORM,
		R8_SNORM,
		RG8_UINT,
		RG8_SINT,
		RG8_UNORM,
		RG8_SNORM,
		R16_UINT,
		R16_SINT,
		R16_UNORM,
		R16_SNORM,
		R16_FLOAT,
		BGRA4_UNORM,
		B5G6R5_UNORM,
		B5G5R5A1_UNORM,
		RGBA8_UINT,
		RGBA8_SINT,
		RGBA8_UNORM,
		RGBA8_SNORM,
		BGRA8_UNORM,
		RGB10A2_UNORM,
		R11G11B10_FLOAT,
		RG16_UINT,
		RG16_SINT,
		RG16_UNORM,
		RG16_SNORM,
		RG16_FLOAT,
		R32_UINT,
		R32_SINT,
		R32_FLOAT,
		RGBA16_UINT,
		RGBA16_SINT,
		RGBA16_FLOAT,
		RGBA16_UNORM,
		RGBA16_SNORM,
		RG32_UINT,
		RG32_SINT,
		RG32_FLOAT,
		RGB32_UINT,
		RGB32_SINT,
		RGB32_FLOAT,
		RGBA32_UINT,
		RGBA32_SINT,
		RGBA32_FLOAT,

		R9G9B9E5_SHAREDEXP,

		BC1_UNORM,
		BC2_UNORM,
		BC3_UNORM,
		BC4_UNORM,
		BC4_SNORM,
		BC5_UNORM,
		BC5_SNORM,
		BC6H_UFLOAT,
		BC6H_SFLOAT,
		BC7_UNORM,

		D16_UNORM,
		D32_FLOAT,
		D24S8,
		D32S8,

		_COUNT,
	};

	enum class FenceFlag
	{
		None               = 0,
		Shared             = RYU_ENUM_BIT(0),
		SharedCrossAdapter = RYU_ENUM_BIT(1),
		Monitored          = RYU_ENUM_BIT(2)
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(FenceFlag)

	enum class CmdListType
	{
		None = -1,
		Direct = 0,
		Bundle,
		Compute,
		Copy,
		VideoDecode,
		VideoProcess,
		VideoEncode,

		_COUNT = (VideoEncode + 1)
	};
	
	enum class CmdListFlags
	{
		None              = 0,
		DisableGPUTimeout = RYU_ENUM_BIT(0)
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(CmdListFlags)

	enum class CmdQueuePriority : i32
	{
		Normal   = 0,
		High     = 100,
		Realtime = 10000
	};

	enum class DescHeapType
	{
		CBV_SRV_UAV = 0,
		Sampler,
		RTV,
		DSV,

		_COUNT
	};

	enum class DescHeapFlags
	{
		None          = 0,
		ShaderVisible = RYU_ENUM_BIT(0)
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(DescHeapFlags)

	enum class TextureFlags
	{
		None            = 0,
		UnorderedAccess = RYU_ENUM_BIT(0),
		ShaderResource  = RYU_ENUM_BIT(1),
		RenderTarget    = RYU_ENUM_BIT(2),
		DepthStencil    = RYU_ENUM_BIT(3),
		SRGB            = RYU_ENUM_BIT(4),
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(TextureFlags)

	enum class TextureType
	{
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
		TextureCubeArray,
	};
}
