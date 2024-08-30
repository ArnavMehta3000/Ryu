#include "StackTrace.h"
#include <Core/ObjectMacros.h>
#include <sstream>
#include <backward/backward.hpp>

namespace Ryu
{
	namespace bw = backward;

	StackTrace::StackTrace()
	{
		bw::TraceResolver _workaround;  // Github issue #206
		m_trace = std::make_unique<bw::StackTrace>();
		m_trace->load_here(64);
		m_trace->skip_n_firsts(2);
	}

	StackTrace::StackTrace(MAYBE_UNUSED const StackTrace& other)
		: m_trace(std::make_unique<bw::StackTrace>(*m_trace))
	{
	}

	StackTrace& StackTrace::operator=(MAYBE_UNUSED const StackTrace& other)
	{
		m_trace = std::make_unique<bw::StackTrace>(*m_trace);
		return *this;
	}

	StackTrace::~StackTrace() = default;

	std::string StackTrace::Print() const
	{
		std::ostringstream oss;
		bw::Printer printer;
		printer.print(*m_trace, oss);
		return oss.str();
	}
}
