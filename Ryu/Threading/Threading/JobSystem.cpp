#include "Threading/JobSystem.h"

namespace Ryu::MT
{
    JobSystem::JobSystem(size_t workerThreads)
		: m_pool(workerThreads)
    {
    }
    
    void JobSystem::WaitForAll(std::span<std::shared_ptr<JobHandle>> handles)
    {
        for (const auto& handle : handles)
        {
            handle->Wait();
        }
    }

    void JobSystem::ProcessReadyJobs()
    {
        std::lock_guard lock(m_mutex);

        std::queue<std::unique_ptr<Job>> remaining;

        while (!m_pendingJobs.empty())
        {
            std::unique_ptr<Job> job = std::move(m_pendingJobs.front());
            m_pendingJobs.pop();

            if (job->AreDependenciesReady())
            {
                // Capture weak reference to this JobSystem
                m_pool.Submit([jobPtr = std::move(job), this]
                {
                    jobPtr->Execute();
                    ProcessReadyJobs(); // Check for newly ready jobs
                });
            }
            else
            {
                remaining.push(std::move(job));
            }
        }

        m_pendingJobs = std::move(remaining);
    }
}
