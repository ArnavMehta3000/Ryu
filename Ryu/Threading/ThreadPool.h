#pragma once
#include "Threading/Job.h"
#include <queue>
#include <future>

namespace Ryu::MT
{
	class ThreadPool
	{
	public:
		explicit ThreadPool(u64 numThreads = std::thread::hardware_concurrency());
		~ThreadPool();

		void Shutdown();
		u64 GetNumThreads() const;

		template<typename Func, typename... Args>
		auto Submit(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>;

	private:
		void WorkerLoop(std::stop_token token);

	private:
		std::vector<std::jthread>         m_workers;
		std::queue<std::function<void()>> m_tasks;
		std::mutex                        m_mutex;
		std::condition_variable_any       m_cv;
		std::atomic<bool>                 m_stopRequested{ false };
	};
}

#include "Threading/ThreadPool.inl"
