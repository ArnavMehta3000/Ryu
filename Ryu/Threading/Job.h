#pragma once
#include "Threading/JobHandle.h"
#include <functional>
#include <memory>
#include <vector>

namespace Ryu::MT
{
	class Job
	{
	public:
		template <typename Func>
		Job(Func&& func, std::vector<std::shared_ptr<JobHandle>> dependencies = {})
			: m_function(std::forward<Func>(func))
			, m_dependencies(std::move(dependencies))
			, m_handle(std::make_shared<JobHandle>())
		{
		}

		void Execute();
		bool AreDependenciesReady() const;

		inline std::shared_ptr<JobHandle> GetHandle() const { return m_handle; }

	private:
		std::function<void()>                   m_function;
		std::vector<std::shared_ptr<JobHandle>> m_dependencies;
		std::shared_ptr<JobHandle>              m_handle;
	};
}
