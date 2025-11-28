#include "Threading/ThreadPool.h"

namespace Ryu::MT
{
    ThreadPool::ThreadPool(u64 numThreads)
    {
        m_workers.reserve(numThreads);

        for (u64 i = 0; i < numThreads; i++)
        {
            m_workers.emplace_back([this](std::stop_token token) { WorkerLoop(token); });
        }
    }

    ThreadPool::~ThreadPool()
    {
        Shutdown();
    }

    void ThreadPool::Shutdown()
    {
        {
            std::lock_guard lock(m_mutex);
            m_stopRequested.store(true);
        }

        m_cv.notify_all();

        for (auto& worker : m_workers)
        {
            worker.request_stop();
            if (worker.joinable())
			{
				worker.join();
			}
        }

		m_workers.clear();
    }

    u64 ThreadPool::GetNumThreads() const
    {
        return m_workers.size();
    }
    
    void ThreadPool::WorkerLoop(std::stop_token token)
    {
        while (!token.stop_requested())
        {
            std::function<void()> task;
            
            {
                std::unique_lock lock(m_mutex);
				m_cv.wait(lock, token, [this] { return !m_tasks.empty() || m_stopRequested.load(); });

                if (token.stop_requested() || (m_tasks.empty() && m_stopRequested))
                {
                    return;
                }

				task = std::move(m_tasks.front());
				m_tasks.pop();
            }

            task();
        }
    }
}
