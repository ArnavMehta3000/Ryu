#pragma once
#include "Common/Common.h"

#include <CmdListFLags.Generated.h>
#include <CmdListType.Generated.h>
#include <CmdQueuePriority.Generated.h>
#include <DescHeapFlags.Generated.h>
#include <DescHeapType.Generated.h>
#include <FenceFlag.Generated.h>
#include <Format.Generated.h>
#include <TextureFlags.Generated.h>
#include <TextureType.Generated.h>

namespace Ryu::Gfx
{
	class Device;
	class DeviceObject;
	class SwapChain;
	class CmdQueue;
	class CommandContext;
	class DescriptorHeap;

	constexpr u32 FRAME_BUFFER_COUNT = 2;
	constexpr Format BACK_BUFFER_FORMAT = Format::RGBA16_FLOAT;
	constexpr Format DEPTH_STENCIL_FORMAT = Format::D24S8;
}
