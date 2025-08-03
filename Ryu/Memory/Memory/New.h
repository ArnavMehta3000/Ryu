#pragma once

namespace Ryu::Memory
{
	size_t GetTotalAllocated();
	size_t GetCurrentUsage();
	size_t GetPeakUsage();
	size_t GetAllocationCount();
	size_t GetDeallocationCount();
}
