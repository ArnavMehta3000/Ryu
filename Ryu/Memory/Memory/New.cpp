#include "Memory/New.h"
#include <new>
#include <cassert>

/* --------------------------------------------------------------------------------------
I am storing the size of the allocation in the first sizeof(size_t) bytes of the allocation.
----------------------------------------------------------------------------------------- */

// Change this to 0 to disable memory stats
#define RYU_ENABLE_MEMORY_STATS 1

static Ryu::Memory::UsageStats g_stats{};

namespace
{
    // Alignment for size_t storage (typically 8 bytes on 64-bit systems)
    constexpr size_t SIZE_STORAGE_ALIGN = alignof(size_t);
    constexpr size_t SIZE_STORAGE_SIZE = sizeof(size_t);
    constexpr size_t SIZE_ALIGNED_STORAGE = (SIZE_STORAGE_SIZE + SIZE_STORAGE_ALIGN - 1) & ~(SIZE_STORAGE_ALIGN - 1);

    // Calculate total allocation size including size storage
    constexpr size_t GetTotalAllocationSize(size_t requestedSize)
    {
        return SIZE_ALIGNED_STORAGE + requestedSize;
    }

    // Store size in the allocated memory
    constexpr void StoreSize(void* rawPtr, size_t size)
    {
        *static_cast<size_t*>(rawPtr) = size;
    }

    // Retrieve size from allocated memory
    constexpr size_t GetStoredSize(void* userPtr)
    {
        void* rawPtr = static_cast<char*>(userPtr) - SIZE_ALIGNED_STORAGE;
        return *static_cast<size_t*>(rawPtr);
    }

    // Get user pointer from raw allocation
    constexpr void* GetUserPtr(void* rawPtr)
    {
        return static_cast<char*>(rawPtr) + SIZE_ALIGNED_STORAGE;
    }

    // Get raw pointer from user pointer
    constexpr void* GetRawPtr(void* dataPtr)
    {
        return static_cast<char*>(dataPtr) - SIZE_ALIGNED_STORAGE;
    }
}

#pragma region Getters
bool Ryu::Memory::IsMemoryTrackingEnabled()
{
#if RYU_ENABLE_MEMORY_STATS
	return true;
#else
	return false;
#endif
}

size_t Ryu::Memory::GetTotalAllocated()
{
    return g_stats.TotalAllocated.load();
}

size_t Ryu::Memory::GetCurrentUsage()
{
    return g_stats.CurrentUsage.load();
}

size_t Ryu::Memory::GetPeakUsage()
{
    return g_stats.PeakUsage.load();
}

size_t Ryu::Memory::GetAllocationCount()
{
    return g_stats.AllocationCount.load();
}

size_t Ryu::Memory::GetDeallocationCount()
{
    return g_stats.DeallocationCount.load();
}
#pragma endregion

// Override new/delete operations
#if RYU_ENABLE_MEMORY_STATS
[[nodiscard]] void* operator new(size_t size)
{
    size_t totalSize = GetTotalAllocationSize(size);
    void* rawPtr = std::malloc(totalSize);
    if (!rawPtr)
    {
        throw std::bad_alloc();
    }

    StoreSize(rawPtr, size);
    void* userPtr = GetUserPtr(rawPtr);

    g_stats.AddAllocation(size);

    return userPtr;
}

[[nodiscard]] void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    size_t totalSize = GetTotalAllocationSize(size);
    void* rawPtr = std::malloc(totalSize);
    if (!rawPtr)
    {
        return nullptr;
    }

    StoreSize(rawPtr, size);
    void* userPtr = GetUserPtr(rawPtr);

    g_stats.AddAllocation(size);

    return userPtr;
}

void operator delete(void* ptr) noexcept
{
    if (!ptr)
    {
        return;
    }

    size_t size = GetStoredSize(ptr);
    void* rawPtr = GetRawPtr(ptr);

    g_stats.RemoveAllocation(size);
    std::free(rawPtr);
}

void operator delete(void* ptr, size_t size) noexcept
{
    if (!ptr)
    {
        return;
    }

#if defined(RYU_BUILD_DEBUG)
    size_t stored_size = GetStoredSize(ptr);
    if (stored_size != size)
    {
        assert(false && "Provided size does not match stored size");
    }
#endif

    void* rawPtr = GetRawPtr(ptr);
    g_stats.RemoveAllocation(size);
    std::free(rawPtr);
}

[[nodiscard]] void* operator new[](size_t size) { return operator new(size); }
[[nodiscard]] void* operator new[](size_t size, const std::nothrow_t&) noexcept { return operator new(size, std::nothrow); }

void operator delete(void* ptr, const std::nothrow_t&) noexcept { operator delete(ptr); }
void operator delete(void* ptr, size_t size, const std::nothrow_t&) noexcept { operator delete(ptr, size); }

void operator delete[](void* ptr) noexcept { operator delete(ptr); }
void operator delete[](void* ptr, const std::nothrow_t&) noexcept { operator delete(ptr); }
void operator delete[](void* ptr, size_t size) noexcept { operator delete(ptr, size); }
void operator delete[](void* ptr, size_t size, const std::nothrow_t&) noexcept { operator delete(ptr, size); }
#endif
