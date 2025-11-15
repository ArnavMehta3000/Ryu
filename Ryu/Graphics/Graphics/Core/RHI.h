#pragma once
#include "Common/Common.h"
#include <array>

namespace Ryu::Gfx
{
	constexpr u32 TIMEOUT_DURATION        = 5000;
	constexpr u32 FRAME_BUFFER_COUNT      = 2;

	// Array of frame resources
	template <typename T>
	using FrameArray = std::array<T, FRAME_BUFFER_COUNT>;

	// Array of ComPtr frame resources
	template <typename T>
	using ComFrameArray = std::array<ComPtr<T>, FRAME_BUFFER_COUNT>;
}
