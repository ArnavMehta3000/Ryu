#pragma once
#include "Common/StandardTypes.h"
#include "Logger/Assert.h"
#include <vector>

namespace Ryu::Gfx
{
	/*
	This free list has an issue that if `T` has a destructor (non-trivial type),
	On destruction of the free list, the dtor of the std::vector will be called which will
	in turn call the dtor of the type `T`. This may be a problem since when we 'Remove', we
	mark the memory to be uninitialized.
	// NOTE if this causes problems in the future, I'll have to fix it, for now we're gucci!
	*/

	template <typename T>
	concept FreeListType = requires { sizeof(T) >= sizeof(u32); };


	template <FreeListType T>
	class FreeList
	{
	public:
		static constexpr u32 InvalidIndex = static_cast<u32>(-1);
	
	public:
		FreeList() = default;
		
		explicit FreeList(u32 count)
		{
			m_storage.reserve(count);
		}
		
		~FreeList()
		{
			RYU_ASSERT(!m_size, "FreeList<T> was not empty on destruction!");
		}
		
		template <typename... Args> 
		constexpr u32 Add(Args&&... args);

		constexpr void Remove(u32 id);
		constexpr inline u32 GetSize() const { return m_size; }
		constexpr inline u32 GetCapacity() const { return static_cast<u32>(m_storage.size()); }
		constexpr inline bool IsEmpty() const { return m_size == 0; }

		NODISCARD constexpr T& operator[](u32 id)
		{
			RYU_ASSERT(id < m_storage.size() && !AlreadyRemoved(id));
			return m_storage[id];
		}

		NODISCARD constexpr const T& operator[](u32 id) const
		{
			RYU_ASSERT(id < m_storage.size() && !AlreadyRemoved(id));
			return m_storage[id];
		}

	private:
		constexpr bool AlreadyRemoved(u32 id);

	private:
		std::vector<T> m_storage;
		u32            m_nextFreeIndex = InvalidIndex;
		u32            m_size = 0;
	};
}

#include "Graphics/Containers/FreeList.inl"
