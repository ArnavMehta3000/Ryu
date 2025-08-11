#pragma once
#include <atomic>

namespace Ryu::Memory
{
	struct UsageStats
	{
		std::atomic<size_t> TotalAllocated{ 0 };
		std::atomic<size_t> CurrentUsage{ 0 };
		std::atomic<size_t> PeakUsage{ 0 };
		std::atomic<size_t> AllocationCount{ 0 };
		std::atomic<size_t> DeallocationCount{ 0 };

		void UpdatePeakUsage(size_t current)
		{
			size_t peak = PeakUsage.load();
			while (current > peak && !PeakUsage.compare_exchange_weak(peak, current))
			{
				// Keep trying until we update peak or current is no longer greater
			}
		}

		void AddAllocation(size_t size)
		{
			TotalAllocated += size;
			CurrentUsage += size;
			UpdatePeakUsage(CurrentUsage.load());
			AllocationCount++;
		}

		void RemoveAllocation(size_t size)
		{
			CurrentUsage -= size;
			DeallocationCount++;
		}

		void RemoveAllocation()
		{
			DeallocationCount++;
		}
	};

	size_t GetTotalAllocated();
	size_t GetCurrentUsage();
	size_t GetPeakUsage();
	size_t GetAllocationCount();
	size_t GetDeallocationCount();
}
