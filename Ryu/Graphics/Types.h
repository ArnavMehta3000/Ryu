#pragma once
#include <Core/StandardTypes.h>

namespace Ryu::Graphics
{
	struct DescriptorSizes
	{
		u32 RtvDescriptorSize{ 0 };
		u32 DsvDescriptorSize{ 0 };
		u32 CbvSrvUavDescriptorSize{ 0 };
	};
}
