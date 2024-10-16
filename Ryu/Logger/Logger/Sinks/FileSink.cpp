#include "FileSink.h"
#include "Common/ObjectMacros.h"
#include <string>

namespace Ryu::Logging
{
	FileSink::FileSink(const std::string_view& filename)
		: m_file(filename.data(), std::ios::app)
	{
	}

	void FileSink::Log(MAYBE_UNUSED LogLevel level, const std::string& message) const
	{
		if (m_file)
		{
			m_file << message;
			m_file.flush();
		}
	}

	std::string_view FileSink::GetName() const
	{
		return "File";
	}
}
