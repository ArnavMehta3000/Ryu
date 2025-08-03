#include "Memory/New.h"
#include <new>
#include <atomic>

static std::atomic<size_t> g_totalAllocated{ 0 };
static std::atomic<size_t> g_currentUsage{ 0 };
static std::atomic<size_t> g_peakUsage{ 0 };
static std::atomic<size_t> g_allocationCount{ 0 };
static std::atomic<size_t> g_deallocationCount{ 0 };

size_t Ryu::Memory::GetTotalAllocated()
{
    return g_totalAllocated.load();
}

size_t Ryu::Memory::GetCurrentUsage()
{
    return g_currentUsage.load();
}

size_t Ryu::Memory::GetPeakUsage()
{
    return g_peakUsage.load();
}

size_t Ryu::Memory::GetAllocationCount()
{
    return g_allocationCount.load();
}

size_t Ryu::Memory::GetDeallocationCount()
{
    return g_deallocationCount.load();
}

static void UpdatePeakUsage(size_t current)
{
    size_t peak = g_peakUsage.load();
    while (current > peak && !g_peakUsage.compare_exchange_weak(peak, current))
    {
        // Keep trying until we update peak or current is no longer greater
    }
}

[[nodiscard]] void* operator new(size_t size)
{
    void* ptr = std::malloc(size);
    if (!ptr)
    {
        throw std::bad_alloc();
    }

    g_totalAllocated += size;
    g_currentUsage += size;
    g_allocationCount++;

    UpdatePeakUsage(g_currentUsage.load());

    return ptr;
}

[[nodiscard]] void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(size);
    if (ptr)
    {
        g_totalAllocated += size;
        g_currentUsage += size;
        g_allocationCount++;
        UpdatePeakUsage(g_currentUsage.load());
    }
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    if (ptr)
    {
        g_deallocationCount++;
        std::free(ptr);
    }
}

void operator delete(void* ptr, size_t size) noexcept
{
    if (ptr)
    {
        g_currentUsage -= size;
        g_deallocationCount++;
        std::free(ptr);
    }
}

[[nodiscard]] void* operator new[](size_t size) { return operator new(size); }

[[nodiscard]] void* operator new[](size_t size, const std::nothrow_t&) noexcept { return operator new(size, std::nothrow); }

void operator delete[](void* ptr) noexcept { operator delete(ptr); }

void operator delete(void* ptr, const std::nothrow_t&) noexcept { operator delete(ptr); }

void operator delete[](void* ptr, const std::nothrow_t&) noexcept { operator delete(ptr); }

void operator delete[](void* ptr, size_t size) noexcept { operator delete(ptr, size); }
