#include "Threading/JobHandle.h"

	namespace Ryu::MT
	{
		JobHandle::JobHandle()
			: m_isCompleted(false)
		{
		}

		void JobHandle::Wait() const
		{
			std::unique_lock lock(m_mutex);
			m_cv.wait(lock, [this] { return m_isCompleted.load(); });
		}
		
		bool JobHandle::IsReady() const noexcept
		{
			return m_isCompleted.load();
		}

		void JobHandle::Complete()
		{
			m_isCompleted.store(true);
			m_cv.notify_all();
		}
	}
