#pragma once
#include <thread>
#include <mutex>
#include <array>

namespace Ryu::MT
{
	/* Ref: https://wickedengine.net/2018/11/simple-job-system-using-standard-c/ */

	// Thread-safe ring buffer
	template <typename T, size_t capacity>
	class TSRingBuffer
	{
	public:
		inline bool PushBack(const T& item)
		{
			bool result = false;

			{
				std::scoped_lock lock(m_mutex);

				size_t next = (m_head + 1) % capacity;
				if (next != tail)
				{
					data[head] = item;
					head       = next;
					result     = true;
				}
			}

			return result;
		}

		inline bool PopFront(T& item)
		{
			bool result = false;

			{
				std::scoped_lock lock(m_mutex);
				if (m_tail != m_head)
				{
					item   = m_data[m_tail];
					m_tail = (m_tail + 1) % capacity;
					result = true;
				}
			}

			return result;
		}

	private:
		std::array<T, capacity> m_data;
		size_t m_head = 0;
		size_t m_tail = 0;
		std::mutex m_mutex;
	};
}
