#pragma once
#include <map>

namespace Ryu::Gfx
{
	class Buffer;
	class Device;

	// Specialized constant buffer pool with size-based buckets
	class ConstantBufferPool
	{
	public:
		//using BufferPoolMap = std::map<u32, std::unique_ptr<ResourcePool<Buffer>>>;

		struct PoolStats
		{
			u32 BufferSize       = 0;
			u32 TotalBuffers     = 0;
			u32 InUseBuffers     = 0;
			u32 AvailableBuffers = 0;
		};

	public:
		ConstantBufferPool(Device* device, u32 maxFramesforeReclaim = 2);
		Buffer* Acquire(u32 sizeInBytes, u64 currentFrame, const std::string& name = "PooledCB");
		void Release(Buffer* buffer);
		void UpdateBuffer(Buffer* buffer, const void* data, u32 sizeInBytes, bool unmap = true);
		void Reclaim(u64 currentFrame);
		void ResetFrame(u64 currentFrame);
		std::vector<PoolStats> GetStats() const;
		void Clear();

	private:
		//ResourcePool<Buffer>& GetOrCreatePool(u32 size);

	private:
		Device*       m_device = nullptr;
		//BufferPoolMap m_pools;
		u32           m_maxFramesBeforeReclaim = 0;
	};

	//using ScopedConstantBuffer = ScopedPooledResource<Buffer, ConstantBufferPool>;
}
