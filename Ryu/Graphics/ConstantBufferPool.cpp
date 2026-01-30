#include "Graphics/ConstantBufferPool.h"

namespace Ryu::Gfx
{
	ConstantBufferPool::ConstantBufferPool(Device* device, u32 maxFramesforeReclaim)
		: m_device(device)
		, m_maxFramesBeforeReclaim(maxFramesforeReclaim)
	{
		//constexpr u32 commonSizes[] = { 256, 512, 1024, 2048, 4096, 8192, 16384 };
		//for (u32 size : commonSizes)
		//{
		//	//m_pools[size] = std::make_unique<ResourcePool<Buffer>>(16, maxFramesforeReclaim);
		//}
	}

	Buffer* ConstantBufferPool::Acquire(u32 sizeInBytes, u64 currentFrame, const std::string& name)
	{
		//const u32 alignedSize = Buffer::CalculateConstantBufferSize(sizeInBytes);

		//auto& pool = GetOrCreatePool(alignedSize);

		// Acquire buffer from pool
		//return pool.Acquire(currentFrame, [this, alignedSize, &name]()
		//{
		//	Buffer::Desc desc{};
		//	desc.SizeInBytes = alignedSize;
		//	desc.Usage       = Buffer::Usage::Upload; // Upload heap for dynamic updates
		//	desc.Type        = Buffer::Type::Constant;
		//	desc.Name        = name;

		//	return std::make_unique<Buffer>(m_device, desc);
		//});
		return nullptr;
	}

	void ConstantBufferPool::Release(Buffer* buffer)
	{
		if (!buffer)
		{
			return;
		}

		/*const u32 size = buffer->GetDesc().SizeInBytes;
		auto it = m_pools.find(size);
		if (it != m_pools.end())
		{
			it->second->Release(buffer);
		}*/
	}

	void ConstantBufferPool::UpdateBuffer(Buffer* buffer, const void* data, u32 sizeInBytes, bool unmap)
	{
		//RYU_ASSERT(buffer, "Buffer is null");
		//RYU_ASSERT(data, "Data is null");

		/*if (buffer && data)
		{
			RYU_ASSERT(sizeInBytes <= buffer->GetDesc().SizeInBytes, "Data size exceeds buffer size");

			void* mapped = buffer->Map();
			std::memcpy(mapped, data, sizeInBytes);
		}

		if (unmap)
		{
			buffer->Unmap();
		}*/
	}

	void ConstantBufferPool::Reclaim(u64 currentFrame)
	{
		//for (auto& [size, pool] : m_pools)
		//{
		//	pool->Reclaim(currentFrame);
		//}
	}

	void ConstantBufferPool::ResetFrame(u64 currentFrame)
	{
		//for (auto& [size, pool] : m_pools)
		//{
		//	pool->ResetFrame(currentFrame);
		//}
	}	//

	std::vector<ConstantBufferPool::PoolStats> ConstantBufferPool::GetStats() const
	{
		std::vector<PoolStats> stats;
		/*stats.reserve(m_pools.size());

		for (const auto& [size, pool] : m_pools)
		{
			auto poolStats = pool->GetStats();

			PoolStats ps
			{
				.BufferSize       = size,
				.TotalBuffers     = poolStats.TotalResources,
				.InUseBuffers     = poolStats.InUseResources,
				.AvailableBuffers = poolStats.AvailableResources,
			};
			stats.push_back(ps);
		}*/

		return stats;
	}

	void ConstantBufferPool::Clear()
	{
		//m_pools.clear();
	}

	/*ResourcePool<Buffer>& ConstantBufferPool::GetOrCreatePool(u32 size)
	{
		auto it = m_pools.find(size);
		if (it == m_pools.end())
		{
			it = m_pools.emplace(size, std::make_unique<ResourcePool<Buffer>>(8, m_maxFramesBeforeReclaim)).first;
		}
		return *it->second;
	}*/
}
