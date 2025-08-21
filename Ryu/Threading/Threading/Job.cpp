#include "Threading/Job.h"
#include <algorithm>

namespace Ryu::MT
{
	void Job::Execute()
	{
		m_function();
		m_handle->Complete();
	}
	
	bool Job::AreDependenciesReady() const
	{
		return std::ranges::all_of(m_dependencies,
			[](const auto& dep) {return dep->IsReady(); });
	}
}
