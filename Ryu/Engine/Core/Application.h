#pragma once
#include <Core/Includes.h>

namespace Ryu
{
	class Application
	{
	public:
		RYU_DISABLE_COPY_AND_MOVE(Application);

		RYU_API Application() = default;
		virtual RYU_API ~Application() = default;
	};
}
