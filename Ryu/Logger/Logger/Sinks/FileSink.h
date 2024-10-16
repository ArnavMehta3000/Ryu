#pragma once
#include "Logger/Sinks/ILogSink.h"
#include <fstream>

namespace Ryu::Logging
{
	class FileSink : public ILogSink
	{
	public:
		explicit FileSink(const std::string_view& filename);
		void Log(LogLevel level, const std::string& message) const override;
		std::string_view GetName() const override;

	private:
		mutable std::ofstream m_file;
	};
}
