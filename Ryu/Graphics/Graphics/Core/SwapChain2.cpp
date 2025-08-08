#include "Graphics/Core/SwapChain2.h"
#include "Logger/Assert.h"

namespace Ryu::Gfx
{
	SwapChain2::~SwapChain2()
	{
		RYU_ASSERT(!m_swapChain, "SwapChain is not released! Call Destroy() first.");
	}
}
