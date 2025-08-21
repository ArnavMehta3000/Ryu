#pragma once
#include "Threading/ThreadPool.h"
#include <span>

namespace Ryu::MT
{
	class JobSystem
	{
	public:
		explicit JobSystem(size_t workerThreads = std::thread::hardware_concurrency() - 1);

		// Submit job with dependencies
		template <typename Func>
		std::shared_ptr<JobHandle> Submit(Func&& func, std::vector<std::shared_ptr<JobHandle>> dependencies = {});

		template <typename Func>
		std::vector<std::shared_ptr<JobHandle>> SubmitParallel(Func&& func, u64 count);

		// Wait for all jobs to complete
		void WaitForAll(std::span<std::shared_ptr<JobHandle>> handles);

		template <typename Iterator, typename Func>
		void ForEach(Iterator first, Iterator last, Func&& func);

	private:
		void ProcessReadyJobs();

	private:
		ThreadPool                       m_pool;
		std::queue<std::unique_ptr<Job>> m_pendingJobs;
		std::mutex                       m_mutex;
	};
}

#include "Threading/JobSystem.inl"
