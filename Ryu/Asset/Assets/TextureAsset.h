#pragma once
#include "Common/StandardTypes.h"
#include <vector>

namespace Ryu::Asset
{
	struct Texture
	{
		Texture(i32 width, i32 height, i32 channels, const std::vector<byte>& data)
			: Width(width)
			, Height(height)
			, Channels(channels)
			, Data(data)
		{ }

        i32 Width = 0;
		i32 Height = 0;
        i32 Channels = 0;
        std::vector<byte> Data;
	};
}
