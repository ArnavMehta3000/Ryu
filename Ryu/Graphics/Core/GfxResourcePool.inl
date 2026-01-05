#include "GfxResourcePool.h"
namespace Ryu::Gfx
{
	template<typename T>
	inline ResourcePool<T>::ResourcePool(u32 initialCapacity, u32 maxFramesBeforeReclaim)
		: m_maxFramesBeforeReclaim(maxFramesBeforeReclaim)
	{
		m_resources.reserve(initialCapacity);
	}

	template<typename T>
	inline T* ResourcePool<T>::Acquire(u64 currentFrame, const ResourcePool<T>::CreateFunc& createFunc)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// Try to reuse an available resource
		for (auto& pooled : m_resources)
		{
			if (!pooled.InUse)
			{
				pooled.InUse = true;
				pooled.LastUsedFrame = currentFrame;
				return pooled.Resource.get();
			}
		}

		// No available resources, create a new one
		PooledResource<T> newPooled
		{
			.Resource      = createFunc(),
			.LastUsedFrame = currentFrame,
			.InUse         = true
		};

		m_resources.push_back(std::move(newPooled));
		return m_resources.back().Resource.get();
	}

	template<typename T>
	inline void ResourcePool<T>::Release(T* resource)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto& pooled : m_resources)
		{
			if (pooled.Resource.get() == resource)
			{
				pooled.InUse = false;
				return;
			}
		}
	}

	template<typename T>
	inline void ResourcePool<T>::Reclaim(u64 currentFrame)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_resources.erase(
			std::remove_if(m_resources.begin(), m_resources.end(),
				[this, currentFrame](const PooledResource<T>& pooled)
				{
					return !pooled.InUse &&
						(currentFrame - pooled.LastUsedFrame) > m_maxFramesBeforeReclaim;
				}),
			m_resources.end()
		);
	}

	template<typename T>
	inline void ResourcePool<T>::ResetFrame(u64 currentFrame)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto& pooled : m_resources)
		{
			if (pooled.InUse)
			{
				pooled.InUse = false;
				pooled.LastUsedFrame = currentFrame;
			}
		}
	}

	template<typename T>
	inline ResourcePool<T>::Stats ResourcePool<T>::GetStats() const
	{
		Stats stats{};
		stats.TotalResources = static_cast<u32>(m_resources.size());

		for (const auto& pooled : m_resources)
		{
			if (pooled.InUse)
			{
				++stats.InUseResources;
			}
			else
			{
				++stats.AvailableResources;
			}
		}

		return stats;
	}

	template<typename T>
	inline void ResourcePool<T>::Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_resources.clear();
	}

	template<typename T>
	inline u32 ResourcePool<T>::GetCapacity() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return static_cast<u32>(m_resources.size());
	}

	template<typename T, typename PoolType>
	inline ScopedPooledResource<T, PoolType>::ScopedPooledResource(PoolType* pool, T* resource)
		: m_pool(pool), m_resource(resource)
	{
	}

	template<typename T, typename PoolType>
	inline ScopedPooledResource<T, PoolType>::~ScopedPooledResource()
	{
		if (m_pool && m_resource)
		{
			m_pool->Release(m_resource);
		}
	}

	template<typename T, typename PoolType>
	inline ScopedPooledResource<T, PoolType>::ScopedPooledResource(ScopedPooledResource&& other) noexcept
		: m_pool(other.m_pool), m_resource(other.m_resource)
	{
		other.m_pool = nullptr;
		other.m_resource = nullptr;
	}

	template<typename T, typename PoolType>
	inline void ScopedPooledResource<T, PoolType>::Release()
	{
		if (m_pool && m_resource)
		{
			m_pool->Release(m_resource);
			m_pool     = nullptr;
			m_resource = nullptr;
		}
	}
}
