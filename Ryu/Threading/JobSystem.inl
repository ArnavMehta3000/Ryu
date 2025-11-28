#include "JobSystem.h"
namespace Ryu::MT
{
	template<typename Func>
	inline std::shared_ptr<JobHandle> JobSystem::Submit(Func&& func, std::vector<std::shared_ptr<JobHandle>> dependencies)
	{
		std::unique_ptr<Job> job = std::make_unique<Job>(std::forward<Func>(func), std::move(dependencies));
		std::shared_ptr<JobHandle> handle = job->GetHandle();

		{
			std::lock_guard lock(m_mutex);
			m_pendingJobs.push(std::move(job));
		}

		ProcessReadyJobs(); // Submit jobs that are ready to the thread pool

		return handle;
	}
	template<typename Func>
	inline std::vector<std::shared_ptr<JobHandle>> JobSystem::SubmitParallel(Func&& func, u64 count)
	{
		std::vector<std::shared_ptr<JobHandle>> handles;
		handles.reserve(count);

		// Submit each job and return the cached handles
		for (size_t i = 0; i < count; ++i)
		{
			handles.push_back(Submit([func, i] { func(i); }));
		}

		return handles;
	}
	
	template<typename Iterator, typename Func>
	inline void JobSystem::ForEach(Iterator first, Iterator last, Func&& func)
	{
		const u64 distance = std::distance(first, last);
		const u64 threadCount = std::min(distance, m_pool.GetNumThreads());
		const u64 chunkSize = distance / threadCount;

		std::vector<std::shared_ptr<JobHandle>> handles;
		handles.reserve(threadCount);

		auto it = first;
		for (u64 i = 0; i < threadCount; i++)
		{
			auto chunkEnd = (i == threadCount - 1) ? last : std::next(it, chunkSize);

			// Submit each chunk
			handles.push_back(Submit(
				[it, chunkEnd, func] ()
				{
					std::for_each(it, chunkEnd, func);
				})
			);

			it = chunkEnd;
		}

		WaitForAll(handles);
	}
}
