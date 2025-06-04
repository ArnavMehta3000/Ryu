#pragma once
#include "Common/StandardTypes.h"
#include <string>

namespace Ryu::Window
{
	struct WindowConfig
	{
		i32 Width              = 1280;
		i32 Height             = 720;
		i32 X                  = static_cast<i32>(0x80000000);  // CW_USEDEFAULT
		i32 Y                  = static_cast<i32>(0x80000000);  // CW_USEDEFAULT
		std::string Title      = "Ryu App Window";
		bool IsResizable       = true;
		bool HasMinimizeButton = true;
		bool HasMaximizeButton = true;
		bool HasCloseButton    = true;
		bool IsVisible         = true;
	};
}
