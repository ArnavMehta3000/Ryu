#pragma once
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace Ryu::MT
{
	class JobHandle
	{
		friend class JobSystem;
		friend class Job;

	public:
		JobHandle();
		
		void Wait() const;
		bool IsReady() const noexcept;

		template <typename Rep, typename Period>
		bool WaitFor(const std::chrono::duration<Rep, Period>& timeout) const;

	private:
		void Complete();

	private:
		mutable std::mutex              m_mutex;
		mutable std::condition_variable m_cv;
		std::atomic<bool>               m_isCompleted;
	};
}

#include "Threading/JobHandle.inl"
