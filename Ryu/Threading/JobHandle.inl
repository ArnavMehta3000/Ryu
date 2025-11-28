namespace Ryu::MT
{
	template<typename Rep, typename Period>
	inline bool JobHandle::WaitFor(const std::chrono::duration<Rep, Period>& timeout) const
	{
		std::unique_lock lock(m_mutex);
		return m_cv.wait_for(lock, timeout, [this] { return m_isCompleted.load(); });
	}
}
