#pragma once
#include "Common/Common.h"
#include <array>
#include <memory>

#include <CommandListFLags.Generated.h>
#include <CommandListType.Generated.h>
#include <CommandQueuePriority.Generated.h>
#include <DescriptorHeapFlags.Generated.h>
#include <DescriptorHeapType.Generated.h>
#include <FenceFlag.Generated.h>
#include <Format.Generated.h>
#include <TextureFlags.Generated.h>
#include <TextureType.Generated.h>
#include <HeapType.Generated.h>

namespace Ryu::Gfx
{
	constexpr u32 TIMEOUT_DURATION        = 5000;
	constexpr u32 FRAME_BUFFER_COUNT      = 2;
	constexpr Format BACK_BUFFER_FORMAT   = Format::RGBA8_UNORM;
	constexpr Format DEPTH_STENCIL_FORMAT = Format::D32_FLOAT;

	// Array of frame resources
	template <typename T>
	using FrameArray = std::array<T, FRAME_BUFFER_COUNT>;

	template <typename T>
	using ComFrameArray = std::array<ComPtr<T>, FRAME_BUFFER_COUNT>;
}
