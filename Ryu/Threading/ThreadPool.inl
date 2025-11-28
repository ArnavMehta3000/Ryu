namespace Ryu::MT
{
	template<typename Func, typename ...Args>
	inline auto ThreadPool::Submit(Func&& func, Args && ...args) -> std::future<std::invoke_result_t<Func, Args ...>>
	{
		using ReturnType = std::invoke_result_t<Func, Args...>;

		auto task = std::make_shared<std::packaged_task<ReturnType()>>(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
		);

		auto future = task->get_future();

		{
			std::lock_guard lock(m_mutex);
			if (m_stopRequested)
			{
				throw std::runtime_error("ThreadPool is shutting down");
			}

			m_tasks.emplace([task]() { (*task)(); });
		}

		m_cv.notify_one();
		return future;
	}
}
