#pragma once
#include "Common/Common.h"

#include <CommandListFLags.Generated.h>
#include <CommandListType.Generated.h>
#include <CommandQueuePriority.Generated.h>
#include <DescriptorHeapFlags.Generated.h>
#include <DescriptorHeapType.Generated.h>
#include <FenceFlag.Generated.h>
#include <Format.Generated.h>
#include <TextureFlags.Generated.h>
#include <TextureType.Generated.h>

namespace Ryu::Gfx
{
	template<typename T> class DeviceObject;

	class Fence;
	class Device;
	class SwapChain;
	class CommandCtx;
	class CommandList;

	constexpr u32 FRAME_BUFFER_COUNT      = 2;
	constexpr Format BACK_BUFFER_FORMAT   = Format::RGBA16_FLOAT;
	constexpr Format DEPTH_STENCIL_FORMAT = Format::D24S8;
}
