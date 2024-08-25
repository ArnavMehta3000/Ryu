#pragma once
#include <Core/Includes.h>

namespace Ryu
{
	struct ApplicationConfig
	{
		i16 Width;
		i16 Height;
		std::string_view Title;
	};

	struct ApplicationState
	{
		bool IsRunning;
		bool IsSuspended;
		i16 Width;
		i16 Height;
		f64 DeltaTime;
	};
}
