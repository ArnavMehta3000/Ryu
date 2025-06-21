#pragma once
#include "Common/StandardTypes.h"
#include "Common/Enum.h"

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
	class SwapChain;
	class CmdQueue;

	constexpr u32 FRAME_BUFFER_COUNT = 2;
}
