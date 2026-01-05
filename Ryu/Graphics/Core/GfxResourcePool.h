#pragma once
#include <mutex>

namespace Ryu::Gfx
{
	template <typename T>
	struct PooledResource
	{
		std::unique_ptr<T> Resource;
		u64 LastUsedFrame = 0;
		bool InUse = false;
	};

	template <typename T>
	class ResourcePool
	{
	public:
		struct Stats
		{
			u32 TotalResources = 0;
			u32 InUseResources = 0;
			u32 AvailableResources = 0;
		};
	public:
		using CreateFunc = std::function<std::unique_ptr<T>()>;

		ResourcePool(u32 initialCapacity = 16, u32 maxFramesBeforeReclaim = 3);
		virtual ~ResourcePool() = default;

		// Get or create a resource
		T* Acquire(u64 currentFrame, const CreateFunc& createFunc);

		// Release a resource back to the pool
		void Release(T* resource);

		// Clean up resources that haven't been used recently
		void Reclaim(u64 currentFrame);

		// Reset all resources (marks them as available)
		void ResetFrame(u64 currentFrame);

		Stats GetStats() const;
		void Clear();
		u32 GetCapacity() const;

	private:
		std::vector<PooledResource<T>> m_resources;
		u32 m_maxFramesBeforeReclaim;
		mutable std::mutex m_mutex;
	};

	template <typename T, typename PoolType>
	class ScopedPooledResource
	{
		RYU_DISABLE_COPY(ScopedPooledResource)
	public:
		ScopedPooledResource(PoolType* pool, T* resource);
		~ScopedPooledResource();
		ScopedPooledResource(ScopedPooledResource&& other) noexcept;
		ScopedPooledResource& operator=(ScopedPooledResource&& other) noexcept
		{
			if (this != &other)
			{
				Release();
				m_pool           = other.m_pool;
				m_resource       = other.m_resource;
				other.m_pool     = nullptr;
				other.m_resource = nullptr;
			}
			return *this;
		}

		void Release();

		T* Get() const                 { return m_resource;            }
		T* operator->() const          { return m_resource;            }
		T& operator*() const           { return *m_resource;           }
		explicit operator bool() const { return m_resource != nullptr; }

	private:
		PoolType* m_pool;
		T*        m_resource;
	};

}

#include "Graphics/Core/GfxResourcePool.inl"
