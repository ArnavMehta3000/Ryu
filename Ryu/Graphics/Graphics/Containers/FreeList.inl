#include "FreeList.h"
namespace Ryu::Gfx
{
	template<FreeListType T> 
	template<typename ...Args>
	inline constexpr u32 FreeList<T>::Add(Args && ...args)
	{
		u32 id = InvalidIndex;

		if (m_nextFreeIndex == InvalidIndex)
		{
			id = static_cast<u32>(m_storage.size());
			m_storage.emplace_back(std::forward<Args>(args)...);
		}
		else
		{
			// The first 4 bytes of an available slot 
			// contains the index to the next available slot
			id = m_nextFreeIndex;
			m_nextFreeIndex = *static_cast<const u32* const>(std::addressof(m_storage[id]));
			new (std::addressof(m_storage[id])) T(std::forward<Args>(args)...);
		}
		
		++m_size;
		return id;
	}

	template<FreeListType T>
	inline constexpr void FreeList<T>::Remove(u32 id)
	{
		// Make sure the index is valid
		RYU_ASSERT(id < m_storage.size() && !AlreadyRemoved(id));

		T& item = m_storage[id];
		item.~T();

		// Mark as uninitialized memory
		RYU_DEBUG_OP(std::memset(std::addressof(item), 0xcc, sizeof(T)));
		
		*static_cast<const u32* const>(std::addressof(m_storage[id])) = m_nextFreeIndex;
		m_nextFreeIndex = id;
		--m_size;
	}
	
	template<FreeListType T>
	inline constexpr bool FreeList<T>::AlreadyRemoved(u32 id)
	{
		if constexpr (sizeof(T) > sizeof(u32))
		{
			// Skip the first 4 bytes
			u32 i = sizeof(u32);
			const u8* const ptr = static_cast<const u8* const>(std::addressof(m_storage[id]));

			while ((ptr[i] == 0xcc) && (i < sizeof(T)))
			{
				++i;
			}
			
			// Complete block of memory (of size T) was marked as uninitialized
			// Which means the item was already removed
			return i == sizeof(T);
		}
		else
		{
			// When the sizes are the same or less, we can't test if the item was already removed
			return true
		}
	}
}
