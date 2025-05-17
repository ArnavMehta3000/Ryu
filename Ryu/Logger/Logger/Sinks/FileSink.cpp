#include "FileSink.h"
#include "Logger/Logger.h"
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

	void FileSink::Log(const LogCategory&, LogLevel level, const LogMessage&, const FormattedLogMessage& formattedMsg) const
	{
		if (m_file)
		{
			m_file << DefaultFormatter(level, formattedMsg) << "\n";
			m_file.flush();
		}
	}

	std::string_view FileSink::GetName() const
	{
		return "File";
	}
}
