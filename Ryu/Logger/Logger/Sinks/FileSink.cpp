#include "FileSink.h"
#include "Common/ObjectMacros.h"
#include <string>
#include <filesystem>

namespace Ryu::Logging
{
	FileSink::FileSink(const std::string_view& filename)
	{
		namespace fs = std::filesystem;
		fs::path path(filename);

		if (!fs::exists(path.parent_path()))
		{
			fs::create_directories(path.parent_path());
		}
		
		m_file = std::ofstream(filename.data(), std::ios::out | std::ios::trunc);
	}

	void FileSink::Log(MAYBE_UNUSED LogLevel level, const LogMessage& message) const
	{
		if (m_file)
		{
			m_file << message.Message << "\n";
			m_file.flush();
		}
	}

	std::string_view FileSink::GetName() const
	{
		return "File";
	}
}
