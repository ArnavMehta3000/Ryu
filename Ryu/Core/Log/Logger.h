#pragma once
#include <CoreDLLDefines.h>
#include <string>

namespace Ryu
{
	class Logger
	{
	public:
		static RYU_API void Initialize(const std::string& name);
		static RYU_API void Shutdown();
	};
}
